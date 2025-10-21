#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include "Lamp.h"
#include "effects/Effect.h"

class HueBridge; // Forward declaration

// Represents a user-defined group of lamps, storing their IDs
using LogicalGroup = QList<int>;

class LampGroupManager : public QObject {
    Q_OBJECT

public:
    explicit LampGroupManager(QObject* parent = nullptr);

    void registerBridge(HueBridge* bridge);
    void addLamp(const Lamp& lamp);
    void createGroup(const QString& groupName);
    void addLampToGroup(const QString& groupName, int lampId);

    // Command sending logic, called by EffectEngine
    void sendSingleRestCommand(int lampId, bool on, int brightness, const QColor& color);
    void sendGroupDtlsStream(const QMap<QString, LightStateMap>& bridgeStates);

    const QMap<int, Lamp>& getAllLamps() const;
    const LogicalGroup& getGroup(const QString& groupName) const;

private:
    QMap<QString, HueBridge*> m_bridges; // Keyed by bridge ID (IP address)
    QMap<int, Lamp> m_allLamps; // Keyed by lamp ID
    QMap<QString, LogicalGroup> m_logicalGroups; // Keyed by group name
};
