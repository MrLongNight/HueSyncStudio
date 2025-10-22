#pragma once

#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class HueBridge;
class LampGroupManager;

/**
 * @class MultiBridgeManager
 * @brief Manages discovery and lifecycle of multiple Hue Bridges.
 *
 * Discovers bridges on the network and creates HueBridge instances
 * for each one found.
 */
class MultiBridgeManager : public QObject {
    Q_OBJECT

public:
    explicit MultiBridgeManager(LampGroupManager& lampManager, QObject* parent = nullptr);

    void discoverBridges();
    void addManualBridge(const QString& ip, const QString& apiKey, const QString& clientKey);
    const QList<HueBridge*>& getBridges() const;

signals:
    void discoveryFinished();

private slots:
    void onDiscoveryFinished(QNetworkReply* reply);

private:
    LampGroupManager& m_lampManager;
    QList<HueBridge*> m_bridges;
    QNetworkAccessManager* m_networkManager;
};

