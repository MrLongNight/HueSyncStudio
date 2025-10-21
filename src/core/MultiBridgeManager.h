#pragma once

#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class HueBridge;

class MultiBridgeManager : public QObject {
    Q_OBJECT

public:
    explicit MultiBridgeManager(QObject* parent = nullptr);

    void discoverBridges();
    const QList<HueBridge*>& getBridges() const;

private slots:
    void onDiscoveryFinished(QNetworkReply* reply);

private:
    QList<HueBridge*> m_bridges;
    QNetworkAccessManager* m_networkManager;
};
