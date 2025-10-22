#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QColor>

using ColorPalette = QList<QColor>;

class ColorGroupEditor : public QObject {
    Q_OBJECT

public:
    explicit ColorGroupEditor(QObject* parent = nullptr);

    void createPalette(const QString& name);
    void addColorToPalette(const QString& name, const QColor& color);
    void clearPalettes();
    const ColorPalette& getPalette(const QString& name) const;
    const QMap<QString, ColorPalette>& getAllPalettes() const;
    const ColorPalette& getPalette(const QString& name) const;

    // The core logic methods will be added in subsequent steps
    QColor getColorFromPalette(const QString& name, double index) const;
    QColor getDeviatedColor(const QColor& color, double deviation) const;

private:
    QMap<QString, ColorPalette> m_palettes;
    static const ColorPalette m_emptyPalette; // For safe return on not found
};
