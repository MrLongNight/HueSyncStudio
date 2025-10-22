#include "MultiBridgeManager.h"
#include "HueBridge.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

MultiBridgeManager::MultiBridgeManager(QObject* parent) : QObject(parent) {
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &MultiBridgeManager::onDiscoveryFinished);
}

void MultiBridgeManager::discoverBridges() {
    Logger::get()->info("Discovering Hue Bridges via Philips discovery endpoint...");
    const QUrl url("https://discovery.meethue.com/");
    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void MultiBridgeManager::onDiscoveryFinished(QNetworkReply* reply) {
    if (reply->error()) {
        Logger::get()->error("Bridge discovery failed: {}", reply->errorString().toStdString());
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (!jsonDoc.isArray()) {
        Logger::get()->error("Bridge discovery response is not a JSON array.");
        reply->deleteLater();
        return;
    }

    qDeleteAll(m_bridges);
    m_bridges.clear();

    const QJsonArray bridgeArray = jsonDoc.array();
    Logger::get()->info("Found {} potential bridge(s).", bridgeArray.size());

    for (const QJsonValue& value : bridgeArray) {
        QJsonObject obj = value.toObject();
        QString ip = obj["internalipaddress"].toString();
        if (!ip.isEmpty()) {
            Logger::get()->info("Discovered Hue Bridge at IP: {}", ip.toStdString());
            HueBridge* bridge = new HueBridge(ip, this);
            m_bridges.append(bridge);
        }
    }

    reply->deleteLater();
}

const QList<HueBridge*>& MultiBridgeManager::getBridges() const {
    return m_bridges;
}
