#include "HueBridge.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslKey>

HueBridge::HueBridge(const QString& ipAddress, QObject* parent)
    : QObject(parent), m_ipAddress(ipAddress), m_dtlsSocket(nullptr) {
    m_networkManager = new QNetworkManager(this);
    m_authTimer = new QTimer(this);
    connect(m_authTimer, &QTimer::timeout, this, &HueBridge::pollForAuthentication);
}

void HueBridge::authenticate() {
    Logger::get()->info("Starting authentication for bridge at {}. Please press the link button.", m_ipAddress.toStdString());
    m_authTimer->start(2000);
}

void HueBridge::pollForAuthentication() {
    const QUrl url(QString("http://%1/api").arg(m_ipAddress));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject requestBody;
    requestBody["devicetype"] = "HueSyncStudio#windows";
    QNetworkReply* reply = m_networkManager->post(request, QJsonDocument(requestBody).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onAuthenticationReply(reply); });
}

void HueBridge::onAuthenticationReply(QNetworkReply* reply) {
    if (reply->error() && reply->error() != QNetworkReply::ProtocolInvalidOperationError) {
        Logger::get()->error("Authentication network error: {}", reply->errorString().toStdString());
    }
    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    const QJsonObject responseObj = jsonDoc.array().first().toObject();
    if (responseObj.contains("success")) {
        m_apiKey = responseObj["success"].toObject()["username"].toString();
        m_authTimer->stop();
        Logger::get()->info("Successfully authenticated with bridge {}. API Key: {}", m_ipAddress.toStdString(), m_apiKey.toStdString());
        emit authenticated(m_apiKey);
    } else if (responseObj.contains("error")) {
        int errorType = responseObj["error"].toObject()["type"].toInt();
        if (errorType == 101) {
            Logger::get()->trace("Link button not pressed on bridge {}. Polling again...", m_ipAddress.toStdString());
        } else {
            QString errorDescription = responseObj["error"].toObject()["description"].toString();
            m_authTimer->stop();
            Logger::get()->error("Authentication failed for bridge {}: {}", m_ipAddress.toStdString(), errorDescription.toStdString());
            emit authenticationFailed(errorDescription);
        }
    }
    reply->deleteLater();
}

void HueBridge::startStreaming(const QString& entertainmentGroupId) {
    if (m_apiKey.isEmpty()) {
        Logger::get()->error("Cannot start streaming: Not authenticated.");
        emit streamingFailed("Not authenticated.");
        return;
    }
    Logger::get()->info("Enabling streaming mode for group {} on bridge {}", entertainmentGroupId.toStdString(), m_ipAddress.toStdString());

    const QUrl url(QString("http://%1/api/%2/groups/%3").arg(m_ipAddress, m_apiKey, entertainmentGroupId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestBody;
    QJsonObject streamObject;
    streamObject["active"] = true;
    requestBody["stream"] = streamObject;

    QNetworkReply* reply = m_networkManager->put(request, QJsonDocument(requestBody).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onEnableStreamingReply(reply); });
}

void HueBridge::onEnableStreamingReply(QNetworkReply* reply) {
    if (reply->error()) {
        Logger::get()->error("Failed to enable streaming mode: {}", reply->errorString().toStdString());
        emit streamingFailed(reply->errorString());
        reply->deleteLater();
        return;
    }

    // Hue API V1 returns credentials in the response to the PUT request. V2 requires a separate fetch.
    // This implementation assumes V2 credentials (PSK) are already known from the entertainment group setup.
    // For this TG, we'll assume we have the PSK and identity. A later TG would fetch them.
    QString pskIdentity = "HueSyncStudioPSK"; // Placeholder, must be 1-32 chars
    QString psk = "deadbeefdeadbeefdeadbeefdeadbeef"; // Placeholder, must be a 32-char hex string

    Logger::get()->info("Streaming mode enabled. Setting up DTLS socket.");
    setupDtlSocket(psk, pskIdentity);
    reply->deleteLater();
}

void HueBridge::setupDtlSocket(const QString& psk, const QString& pskIdentity) {
    m_dtlsSocket = new QSslSocket(this);
    connect(m_dtlsSocket, &QSslSocket::connected, this, &HueBridge::onSocketConnected);
    connect(m_dtlsSocket, &QSslSocket::sslErrors, this, &HueBridge::onSocketSslErrors);

    QSslConfiguration sslConfig = QSslConfiguration::defaultDtlsConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // Hue bridge uses self-signed cert

    // Convert PSK from hex string to byte array
    QByteArray pskBytes = QByteArray::fromHex(psk.toLatin1());
    QSslPresharedKeyAuthenticator authenticator;
    authenticator.setIdentity(pskIdentity.toUtf8());
    authenticator.setPreSharedKey(pskBytes);

    m_dtlsSocket->setDtlsPresharedKeyAuthenticator(authenticator);
    m_dtlsSocket->setSslConfiguration(sslConfig);

    Logger::get()->info("Connecting DTLS socket to {}:2100", m_ipAddress.toStdString());
    m_dtlsSocket->connectToHostEncrypted(m_ipAddress, 2100);
}

void HueBridge::onSocketConnected() {
    Logger::get()->info("DTLS socket successfully connected to bridge {}", m_ipAddress.toStdString());
    emit streamingStarted();
}

void HueBridge::onSocketSslErrors(const QList<QSslError>& errors) {
    for (const QSslError& error : errors) {
        Logger::get()->error("DTLS SSL Error: {}", error.errorString().toStdString());
    }
    emit streamingFailed("SSL handshake failed.");
}
