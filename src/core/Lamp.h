#pragma once

#include <QString>

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
