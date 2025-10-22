#include "WebSocketServer.h"
#include "EffectEngine.h"
#include "LampGroupManager.h"
#include "ColorGroupEditor.h"
#include "effects/PulseEffect.h"
#include "effects/SpectrumEffect.h"
#include "effects/ColorFlowEffect.h"
#include "Logger.h"

#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <memory>

// ... (Constructor and other methods remain the same) ...
WebSocketServer::WebSocketServer(quint16 port,
                                   EffectEngine& effectEngine,
                                   LampGroupManager& lampManager,
                                   ColorGroupEditor& colorEditor,
                                   QObject* parent)
    : QObject(parent),
      m_effectEngine(effectEngine),
      m_lampManager(lampManager),
      m_colorEditor(colorEditor)
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("HueSyncStudio Server"),
                                              QWebSocketServer::NonSecureMode, this);
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        Logger::get()->info("WebSocket server listening on port {}", port);
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &WebSocketServer::onNewConnection);
    } else {
        Logger::get()->error("Failed to start WebSocket server on port {}", port);
    }
}

WebSocketServer::~WebSocketServer() {
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void WebSocketServer::onNewConnection() {
    QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();
    Logger::get()->info("WebSocket client connected: {}", pSocket->peerAddress().toString().toStdString());
    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSocketServer::onSocketDisconnected);
    m_clients << pSocket;
}

void WebSocketServer::onTextMessageReceived(const QString& message) {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    if (!pClient) return;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        pClient->sendTextMessage("{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;
    }
    if (!doc.isObject()) {
        pClient->sendTextMessage("{\"status\":\"error\", \"message\":\"Message must be a JSON object\"}");
        return;
    }
    QJsonObject commandObj = doc.object();
    if (!commandObj.contains("command") || !commandObj["command"].isString()) {
         pClient->sendTextMessage("{\"status\":\"error\", \"message\":\"Message must contain a 'command' string field\"}");
        return;
    }
    processCommand(commandObj, pClient);
}


void WebSocketServer::processCommand(const QJsonObject& command, QWebSocket* pClient) {
    QString commandName = command["command"].toString();
    const QJsonObject payload = command["payload"].toObject();

    Logger::get()->info("Processing WebSocket command: {}", commandName.toStdString());

    if (commandName == "setActiveEffect") {
        QString effectName = payload["effect"].toString();
        Effect* effect = nullptr;
        if (effectName == "Pulse") {
            QColor color = QColor(payload["color"].toString("#FFFFFF"));
            effect = new PulseEffect(color);
        } else if (effectName == "Spectrum") {
            effect = new SpectrumEffect();
        } else if (effectName == "ColorFlow") {
            QString palette = payload["palette"].toString("Default");
            double speed = payload["speed"].toDouble(0.2);
            effect = new ColorFlowEffect(m_colorEditor, palette, speed);
        }

        if (effect) {
            // The engine takes ownership of the raw pointer. This is a simplification.
            // A better design would use a factory and manage lifetime with unique_ptr.
            m_effectEngine.setActiveEffect(effect);
            pClient->sendTextMessage("{\"status\":\"success\", \"command\":\"setActiveEffect\"}");
        } else {
            pClient->sendTextMessage("{\"status\":\"error\", \"message\":\"Unknown effect\"}");
        }

    } else if (commandName == "manualOverride") {
        QString group = payload["group"].toString("All");
        QColor color = QColor(payload["color"].toString("#FFFFFF"));
        int brightness = payload["brightness"].toInt(254);
        m_effectEngine.manualOverride(group, color, brightness);
        pClient->sendTextMessage("{\"status\":\"success\", \"command\":\"manualOverride\"}");

    } else if (commandName == "getAppStatus") {
        QJsonObject status;
        // This part would be expanded to include more data
        status["activeEffect"] = m_effectEngine.getActiveEffectName();
        QJsonDocument doc(status);
        pClient->sendTextMessage(doc.toJson(QJsonDocument::Compact));

    } else {
        pClient->sendTextMessage("{\"status\":\"error\", \"message\":\"Unknown command\"}");
    }
}

void WebSocketServer::onSocketDisconnected() {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    if (pClient) {
        Logger::get()->info("WebSocket client disconnected: {}", pClient->peerAddress().toString().toStdString());
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
