#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QSslSocket>

class HueBridge : public QObject {
    Q_OBJECT

public:
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


private:
    void setupDtlSocket(const QString& psk, const QString& pskIdentity);

    QString m_ipAddress;
    QString m_apiKey;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_authTimer;
    QSslSocket* m_dtlsSocket;
};
