#include "LampGroupManager.h"
#include "HueBridge.h"
#include "Logger.h"
#include <QColor>

LampGroupManager::LampGroupManager(QObject* parent) : QObject(parent) {
    createGroup("All");
}

void LampGroupManager::registerBridge(HueBridge* bridge) {
    if (!bridge) return;
    if (m_bridges.contains(bridge->getId())) {
        Logger::get()->warn("Bridge with ID {} is already registered.", bridge->getId().toStdString());
        return;
    }
    m_bridges[bridge->getId()] = bridge;
    Logger::get()->info("Registered bridge with ID {}", bridge->getId().toStdString());
}

void LampGroupManager::addLamp(const Lamp& lamp) {
    if (m_allLamps.contains(lamp.id)) {
        Logger::get()->warn("Lamp with ID {} already exists. Updating.", lamp.id);
    }
    m_allLamps[lamp.id] = lamp;

    addLampToGroup("All", lamp.id);
    Logger::get()->info("Added/updated lamp ID: {}, Name: {}, Bridge: {}", lamp.id, lamp.name.toStdString(), lamp.bridgeId.toStdString());
}

void LampGroupManager::createGroup(const QString& groupName) {
    if (m_logicalGroups.contains(groupName)) {
        Logger::get()->warn("Logical group '{}' already exists.", groupName.toStdString());
        return;
    }
    m_logicalGroups[groupName] = LogicalGroup();
    Logger::get()->info("Created logical group: {}", groupName.toStdString());
}

void LampGroupManager::addLampToGroup(const QString& groupName, int lampId) {
    if (!m_logicalGroups.contains(groupName)) {
        Logger::get()->error("Cannot add lamp to non-existent group '{}'", groupName.toStdString());
        return;
    }
    if (!m_allLamps.contains(lampId)) {
        Logger::get()->error("Cannot add non-existent lamp with ID {} to group", lampId);
        return;
    }

    if (!m_logicalGroups[groupName].contains(lampId)) {
        m_logicalGroups[groupName].append(lampId);
    }
}

void LampGroupManager::clearGroups() {
    m_logicalGroups.clear();
    // Also re-create the default "All" group
    createGroup("All");
}

void LampGroupManager::sendGroupRestCommand(const QString& groupName, bool on, int brightness) {
    if (!m_logicalGroups.contains(groupName)) {
        Logger::get()->error("Cannot send command to non-existent group '{}'", groupName.toStdString());
        return;
    }

    Logger::get()->info("Sending REST command to group '{}': on={}, bri={}", groupName.toStdString(), on, brightness);

    const LogicalGroup& group = m_logicalGroups[groupName];
    for (int lampId : group) {
        const Lamp& lamp = m_allLamps[lampId];
        if (m_bridges.contains(lamp.bridgeId)) {
            m_bridges[lamp.bridgeId]->sendRestCommand(lamp.id, on, brightness, QColor()); // No specific color
            m_bridges[lamp.bridgeId]->sendRestCommand(lamp.id, on, brightness);
        } else {
            Logger::get()->error("Could not send command to lamp {}: Bridge {} not registered.", lamp.id, lamp.bridgeId.toStdString());
        }
    }
}

void LampGroupManager::sendSingleRestCommand(int lampId, bool on, int brightness, const QColor& color) {
    if (!m_allLamps.contains(lampId)) return;

    const Lamp& lamp = m_allLamps[lampId];
    if (m_bridges.contains(lamp.bridgeId)) {
        m_bridges[lamp.bridgeId]->sendRestCommand(lampId, on, brightness, color);
    }
}

void LampGroupManager::sendGroupDtlsStream(const QMap<QString, LightStateMap>& bridgeStates) {
    for (auto it = bridgeStates.begin(); it != bridgeStates.end(); ++it) {
        const QString& bridgeId = it.key();
        const LightStateMap& state = it.value();
        if (m_bridges.contains(bridgeId)) {
            m_bridges[bridgeId]->sendDtlsStream(state);
        }
    }
}


const QMap<int, Lamp>& LampGroupManager::getAllLamps() const {
    return m_allLamps;
}

const LogicalGroup& LampGroupManager::getGroup(const QString& groupName) const {
    return m_logicalGroups[groupName];
}

const QMap<QString, LogicalGroup>& LampGroupManager::getAllLogicalGroups() const {
    return m_logicalGroups;
}
