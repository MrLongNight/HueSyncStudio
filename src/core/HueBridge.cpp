#include "HueBridge.h"
#include "LampGroupManager.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslKey>

// DTLS Packet structure constants
const char HUE_STREAM_HEADER[] = "HueStream";
const uint8_t PROTOCOL_VERSION_MAJOR = 1;
const uint8_t PROTOCOL_VERSION_MINOR = 0;
const uint8_t SEQUENCE_NUMBER = 1;
const uint16_t RESERVED = 0;
const uint8_t COLOR_SPACE_RGB = 0;
const uint8_t FRAGMENT_NUMBER = 0;

HueBridge::HueBridge(const QString& ipAddress, LampGroupManager& lampManager, QObject* parent)
    : QObject(parent), m_ipAddress(ipAddress), m_lampManager(lampManager), m_dtlsSocket(nullptr) {
    m_networkManager = new QNetworkAccessManager(this);
    m_authTimer = new QTimer(this);
    connect(m_authTimer, &QTimer::timeout, this, &HueBridge::pollForAuthentication);
}

const QString& HueBridge::getId() const {
    return m_ipAddress;
}

const QString& HueBridge::getApiKey() const {
    return m_apiKey;
}

const QString& HueBridge::getClientKey() const {
    return m_clientKey;
}

void HueBridge::setApiKey(const QString& key) {
    m_apiKey = key;
}

void HueBridge::setClientKey(const QString& key) {
    m_clientKey = key;
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
    requestBody["generateclientkey"] = true; // Request a client key for DTLS
    QNetworkReply* reply = m_networkManager->post(request, QJsonDocument(requestBody).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onAuthenticationReply(reply); });
}

void HueBridge::onAuthenticationReply(QNetworkReply* reply) {
    if (reply->error() && reply->error() != QNetworkReply::ProtocolInvalidOperationError) {
        Logger::get()->error("Authentication network error: {}", reply->errorString().toStdString());
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (!jsonDoc.isArray()) {
        Logger::get()->error("Unexpected authentication reply format from bridge {}", m_ipAddress.toStdString());
        reply->deleteLater();
        return;
    }

    const QJsonObject responseObj = jsonDoc.array().first().toObject();

    if (responseObj.contains("success")) {
        QJsonObject successObj = responseObj["success"].toObject();
        m_apiKey = successObj["username"].toString();
        m_clientKey = successObj["clientkey"].toString();
        m_authTimer->stop();
        Logger::get()->info("Successfully authenticated with bridge {}. API Key acquired.", m_ipAddress.toStdString());
        emit authenticated(m_apiKey);
        fetchEntertainmentGroups();
    } else if (responseObj.contains("error")) {
        int errorType = responseObj["error"].toObject()["type"].toInt();
        if (errorType == 101) {
            // Link button not pressed yet -- keep polling
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

void HueBridge::fetchEntertainmentGroups() {
    Logger::get()->info("Fetching entertainment groups from bridge {}", m_ipAddress.toStdString());
    const QUrl url(QString("http://%1/api/%2/groups").arg(m_ipAddress, m_apiKey));
    QNetworkRequest request(url);
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onFetchGroupsReply(reply); });
}

void HueBridge::onFetchGroupsReply(QNetworkReply* reply) {
    if (reply->error()) {
        Logger::get()->error("Failed to fetch groups: {}", reply->errorString().toStdString());
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isObject()) {
        Logger::get()->error("Unexpected groups reply format from bridge {}", m_ipAddress.toStdString());
        reply->deleteLater();
        return;
    }

    const QJsonObject groupsObj = jsonDoc.object();
    m_entertainmentGroups.clear();

    for (auto it = groupsObj.begin(); it != groupsObj.end(); ++it) {
        const QJsonObject group = it.value().toObject();
        if (group["type"].toString() == "Entertainment") {
            QString groupId = it.key();
            QString groupName = group["name"].toString();
            m_entertainmentGroups[groupId] = groupName;
            Logger::get()->info("Found entertainment group: {} (ID: {})", groupName.toStdString(), groupId.toStdString());

            const QJsonObject locations = group["locations"].toObject();
            const QJsonArray lightIds = group["lights"].toArray();
            for (const QJsonValue& lightIdValue : lightIds) {
                QString lightIdStr = lightIdValue.toString();
                Lamp lamp;
                lamp.id = lightIdStr.toInt();
                lamp.name = QString("Light %1").arg(lamp.id);
                if (locations.contains(lightIdStr)) {
                    QJsonArray pos = locations[lightIdStr].toArray();
                    lamp.x = pos.size() > 0 ? pos[0].toDouble() : 0.0;
                    lamp.y = pos.size() > 1 ? pos[1].toDouble() : 0.0;
                    lamp.z = pos.size() > 2 ? pos[2].toDouble() : 0.0;
                } else {
                    lamp.x = lamp.y = lamp.z = 0.0;
                    Logger::get()->warn("Location not found for light ID {} in group {}", lamp.id, groupName.toStdString());
                }
                lamp.bridgeId = m_ipAddress;
                m_lampManager.addLamp(lamp);
            }
        }
    }

    emit entertainmentGroupsFound();
    reply->deleteLater();
}

void HueBridge::startStreaming(const QString& entertainmentGroupId) {
    if (m_apiKey.isEmpty() || m_clientKey.isEmpty()) {
        Logger::get()->error("Cannot start streaming: Not authenticated or client key missing.");
        emit streamingFailed("Not authenticated or client key missing.");
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

    // Assuming the client key / PSK have been set earlier during authentication
    // Setup DTLS socket with the known PSK and identity
    QString pskIdentity = m_apiKey.isEmpty() ? QStringLiteral("HueSyncStudioPSK") : m_apiKey;
    QString psk = m_clientKey.isEmpty() ? QStringLiteral("deadbeefdeadbeefdeadbeefdeadbeef") : m_clientKey;

    Logger::get()->info("Streaming mode enabled. Setting up DTLS socket.");
    setupDtlSocket(psk, pskIdentity);

    reply->deleteLater();
}

void HueBridge::setupDtlSocket(const QString& psk, const QString& pskIdentity) {
    if (m_dtlsSocket) {
        m_dtlsSocket->deleteLater();
        m_dtlsSocket = nullptr;
    }

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

void HueBridge::sendRestCommand(int lightId, bool on, int brightness, const QColor& color) {
    if (m_apiKey.isEmpty()) return;

    const QUrl url(QString("http://%1/api/%2/lights/%3/state").arg(m_ipAddress, m_apiKey, QString::number(lightId)));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject state;
    state["on"] = on;
    if (on) {
        state["bri"] = brightness;
        if (color.isValid()) {
            state["hue"] = static_cast<int>(color.hueF() * 65535);
            state["sat"] = static_cast<int>(color.saturationF() * 254);
        }
    }

    m_networkManager->put(request, QJsonDocument(state).toJson());
}

void HueBridge::sendDtlsStream(const LightStateMap& state) {
    if (!m_dtlsSocket || !m_dtlsSocket->isOpen()) return;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    // Header
    stream.writeRawData(HUE_STREAM_HEADER, 9);
    stream << PROTOCOL_VERSION_MAJOR << PROTOCOL_VERSION_MINOR;
    stream << (uint8_t)m_sequenceId++; // Sequence number
    if (m_sequenceId > 255) m_sequenceId = 1;
    stream << RESERVED;
    stream << COLOR_SPACE_RGB;
    stream << FRAGMENT_NUMBER;

    // Light data
    for (auto it = state.begin(); it != state.end(); ++it) {
        uint8_t lightId = static_cast<uint8_t>(it.key());
        QColor color = it.value();

        stream << (uint8_t)0x00 << lightId; // Type 0 (light ID)
        stream << (uint16_t)(color.red() * 257)      // r
               << (uint16_t)(color.green() * 257)    // g
               << (uint16_t)(color.blue() * 257);     // b
    }

    m_dtlsSocket->write(message);
}