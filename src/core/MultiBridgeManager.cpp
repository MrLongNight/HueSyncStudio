#include "MultiBridgeManager.h"
#include "HueBridge.h"
#include "LampGroupManager.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

MultiBridgeManager::MultiBridgeManager(LampGroupManager& lampManager, QObject* parent)
    : QObject(parent), m_lampManager(lampManager) {
    m_networkManager = new QNetworkManager(this);
    connect(m_networkManager, &QNetworkManager::finished, this, &MultiBridgeManager::onDiscoveryFinished);
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

void MultiBridgeManager::addManualBridge(const QString& ip, const QString& apiKey, const QString& clientKey) {
    Logger::get()->info("Adding manually configured bridge at IP: {}", ip.toStdString());
    HueBridge* bridge = new HueBridge(ip, m_lampManager, this);
    bridge->setApiKey(apiKey);
    bridge->setClientKey(clientKey);

    m_bridges.append(bridge);
    m_lampManager.registerBridge(bridge);

    // Since it's authenticated, directly fetch groups
    QMetaObject::invokeMethod(bridge, "fetchEntertainmentGroups", Qt::QueuedConnection);
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
            HueBridge* bridge = new HueBridge(ip, m_lampManager, this);
            m_bridges.append(bridge);
            m_lampManager.registerBridge(bridge); // Register the bridge
            // In a real app, we would now trigger authentication, e.g., from the UI
            // For now, let's auto-authenticate to trigger the lamp fetching
            bridge->authenticate();
            HueBridge* bridge = new HueBridge(ip, this);
            m_bridges.append(bridge);
        }
    }

    reply->deleteLater();
    emit discoveryFinished();
}

const QList<HueBridge*>& MultiBridgeManager::getBridges() const {
    return m_bridges;
}
