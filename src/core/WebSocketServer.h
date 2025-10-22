#pragma once

#include <QObject>
#include <QList>

class QWebSocketServer;
class QWebSocket;
class EffectEngine;
class LampGroupManager;
class ColorGroupEditor;

class WebSocketServer : public QObject {
    Q_OBJECT

public:
    explicit WebSocketServer(quint16 port,
                               EffectEngine& effectEngine,
                               LampGroupManager& lampManager,
                               ColorGroupEditor& colorEditor,
                               QObject* parent = nullptr);
    ~WebSocketServer();

public:
    // Made public for testing purposes
    void processCommand(const QJsonObject& command, QWebSocket* pClient);

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString& message);
    void onSocketDisconnected();

private:
    QWebSocketServer* m_pWebSocketServer;
    QList<QWebSocket*> m_clients;

    // References to core components
    EffectEngine& m_effectEngine;
    LampGroupManager& m_lampManager;
    ColorGroupEditor& m_colorEditor;
};
