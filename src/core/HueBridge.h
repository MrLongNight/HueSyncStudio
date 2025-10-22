#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QSslSocket>
#include "Lamp.h"
#include "effects/Effect.h"

class LampGroupManager; // Forward declaration

class HueBridge : public QObject {
    Q_OBJECT

public:
    explicit HueBridge(const QString& ipAddress, LampGroupManager& lampManager, QObject* parent = nullptr);

    const QString& getId() const;
    const QString& getApiKey() const;
    const QString& getClientKey() const;

    void setApiKey(const QString& key);
    void setClientKey(const QString& key);

    void authenticate();
    void startStreaming(const QString& entertainmentGroupId);

    // Command methods
    void sendRestCommand(int lightId, bool on, int brightness, const QColor& color);
    void sendDtlsStream(const LightStateMap& state);

    const QMap<QString, QString>& getEntertainmentGroups() const;
    // Placeholder command methods
    void sendRestCommand(int lightId, bool on, int brightness);
    void sendDtlsStream(const QByteArray& data);


signals:
    void authenticated(const QString& apiKey);
    void authenticationFailed(const QString& error);
    void entertainmentGroupsFound();
    void entertainmentGroupsFound(); // To notify when setup is complete
    explicit HueBridge(const QString& ipAddress, QObject* parent = nullptr);

    void authenticate();
    // Pass entertainment group ID to start streaming for that group
    void startStreaming(const QString& entertainmentGroupId);

signals:
    void authenticated(const QString& apiKey);
    void authenticationFailed(const QString& error);
    void streamingStarted();
    void streamingFailed(const QString& error);

private slots:
    void onAuthenticationReply(QNetworkReply* reply);
    void pollForAuthentication();
    void onEnableStreamingReply(QNetworkReply* reply);
    void onSocketConnected();
    void onSocketSslErrors(const QList<QSslError>& errors);
    void onFetchGroupsReply(QNetworkReply* reply);

private:
    void fetchEntertainmentGroups();
    void setupDtlSocket(const QString& psk, const QString& pskIdentity);

    QString m_ipAddress;
    QString m_apiKey;
    QString m_clientKey;
    LampGroupManager& m_lampManager;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_authTimer;
    QSslSocket* m_dtlsSocket;
    uint8_t m_sequenceId = 1;

    QMap<QString, QString> m_entertainmentGroups; // ID -> Name
    QString m_ipAddress; // Also used as a unique ID for the bridge
    QString m_apiKey;
    QString m_clientKey; // The PSK for DTLS
    LampGroupManager& m_lampManager;


private:
    void setupDtlSocket(const QString& psk, const QString& pskIdentity);

    QString m_ipAddress;
    QString m_apiKey;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_authTimer;
    QSslSocket* m_dtlsSocket;
};
