#pragma once

#include <QString>
#include <QMetaType>


struct Lamp {
    int id;
    QString name;
    // Relative position in the entertainment group
    double x;
    double y;
    double z;
    // Reference to the bridge that controls this lamp
    QString bridgeId;
};
#include <QMetaType>

/**
 * @struct Lamp
 * @brief Represents a single Philips Hue light with its properties.
 *
 * This struct stores the identifier, name, and 3D position coordinates (x, y, z)
 * for a single lamp. It is registered with the Qt meta-object system to be
 * usable within QVariant.
 */
struct Lamp {
    int id;
    QString name;
    double x;
    double y;
    double z;
};

Q_DECLARE_METATYPE(Lamp);
