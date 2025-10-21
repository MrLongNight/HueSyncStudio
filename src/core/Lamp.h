#pragma once

#include <QString>
#include <QMetaType>

struct Lamp {
    int id;
    QString name;
    double x;
    double y;
    double z;
};

Q_DECLARE_METATYPE(Lamp);
