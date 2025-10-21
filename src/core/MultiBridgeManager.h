#pragma once

#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class HueBridge;
class LampGroupManager; // Forward declaration

class MultiBridgeManager : public QObject {
    Q_OBJECT

public:
    explicit MultiBridgeManager(LampGroupManager& lampManager, QObject* parent = nullptr);

    void discoverBridges();
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
