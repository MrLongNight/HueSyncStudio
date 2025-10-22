#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include "Lamp.h"
#include "effects/Effect.h"

class HueBridge;

// Represents a user-defined group of lamps, storing their IDs
using LogicalGroup = QList<int>;

/**
 * @class LampGroupManager
 * @brief Manages lamps from multiple bridges and logical lamp groups.
 *
 * Stores all lamps discovered from all bridges, and allows creating
 * logical groups for organized control.
 */
class LampGroupManager : public QObject {
    Q_OBJECT

public:
    explicit LampGroupManager(QObject* parent = nullptr);

    void registerBridge(HueBridge* bridge);
    void addLamp(const Lamp& lamp);
    void createGroup(const QString& groupName);
    void addLampToGroup(const QString& groupName, int lampId);
    void clearGroups();

    void sendSingleRestCommand(int lampId, bool on, int brightness, const QColor& color);
    void sendGroupDtlsStream(const QMap<QString, LightStateMap>& bridgeStates);
    void sendGroupRestCommand(const QString& groupName, bool on, int brightness);

    const QMap<int, Lamp>& getAllLamps() const;
    const LogicalGroup& getGroup(const QString& groupName) const;
    const QMap<QString, LogicalGroup>& getAllLogicalGroups() const;

private:
    QMap<QString, HueBridge*> m_bridges;
    QMap<int, Lamp> m_allLamps;
    QMap<QString, LogicalGroup> m_logicalGroups;
};

