#pragma once

#include <nlohmann/json.hpp>
#include <QColor>
#include "ColorGroupEditor.h" // For ColorPalette
#include "LampGroupManager.h" // For LogicalGroup

using json = nlohmann::json;

// --- QColor ---
inline void to_json(json& j, const QColor& c) {
    j = {{"r", c.red()}, {"g", c.green()}, {"b", c.blue()}};
}

inline void from_json(const json& j, QColor& c) {
    c.setRgb(j.at("r").get<int>(), j.at("g").get<int>(), j.at("b").get<int>());
}

// --- ColorPalette (QList<QColor>) ---
inline void to_json(json& j, const ColorPalette& p) {
    j = json::array();
    for(const auto& color : p) {
        j.push_back(color);
    }
}

inline void from_json(const json& j, ColorPalette& p) {
    p.clear();
    for (const auto& item : j) {
        p.append(item.get<QColor>());
    }
}

// --- LogicalGroup (QList<int>) ---
inline void to_json(json& j, const LogicalGroup& g) {
    j = json::array();
    for(const auto& id : g) {
        j.push_back(id);
    }
}

inline void from_json(const json& j, LogicalGroup& g) {
    g.clear();
    for (const auto& item : j) {
        g.append(item.get<int>());
    }
}
