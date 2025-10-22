#include "ColorGroupEditor.h"
#include "Logger.h"
#include <random>
#include <algorithm>

const ColorPalette ColorGroupEditor::m_emptyPalette = ColorPalette();

ColorGroupEditor::ColorGroupEditor(QObject* parent) : QObject(parent) {
    // Create a default palette for demonstration
    createPalette("Default");
    addColorToPalette("Default", QColor(Qt::red));
    addColorToPalette("Default", QColor(Qt::green));
    addColorToPalette("Default", QColor(Qt::blue));
}

void ColorGroupEditor::createPalette(const QString& name) {
    if (m_palettes.contains(name)) {
        Logger::get()->warn("Color palette '{}' already exists.", name.toStdString());
        return;
    }
    m_palettes[name] = ColorPalette();
    Logger::get()->info("Created color palette: {}", name.toStdString());
}

void ColorGroupEditor::addColorToPalette(const QString& name, const QColor& color) {
    if (!m_palettes.contains(name)) {
        Logger::get()->error("Cannot add color to non-existent palette '{}'", name.toStdString());
        return;
    }
    m_palettes[name].append(color);
}

const ColorPalette& ColorGroupEditor::getPalette(const QString& name) const {
    if (m_palettes.contains(name)) {
        return m_palettes[name];
    }
    Logger::get()->error("Palette '{}' not found.", name.toStdString());
    return m_emptyPalette;
}

QColor ColorGroupEditor::getColorFromPalette(const QString& name, double index) const {
    const ColorPalette& palette = getPalette(name);
    if (palette.isEmpty()) {
        return QColor(Qt::black);
    }
    if (palette.size() == 1) {
        return palette.first();
    }
    index = std::max(0.0, std::min(1.0, index));
    double pos = index * (palette.size() - 1);
    int firstColorIndex = static_cast<int>(floor(pos));
    int secondColorIndex = static_cast<int>(ceil(pos));
    double fraction = pos - firstColorIndex;
    const QColor& c1 = palette[firstColorIndex];
    const QColor& c2 = palette[secondColorIndex];
    int r = static_cast<int>(c1.red() + (c2.red() - c1.red()) * fraction);
    int g = static_cast<int>(c1.green() + (c2.green() - c1.green()) * fraction);
    int b = static_cast<int>(c1.blue() + (c2.blue() - c1.blue()) * fraction);
    return QColor(r, g, b);
}

QColor ColorGroupEditor::getDeviatedColor(const QColor& color, double deviation) const {
    if (deviation <= 0.0) {
        return color;
    }

    // Clamp deviation to be within [0.0, 1.0]
    deviation = std::max(0.0, std::min(1.0, deviation));

    // Convert to HSV for easier manipulation of hue and saturation
    qreal hue, sat, val, alpha;
    color.getHsvF(&hue, &sat, &val, &alpha);

    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // Create a distribution for the deviation range [-deviation, +deviation]
    std::uniform_real_distribution<> distr(-deviation, deviation);

    // Deviate Hue (wraps around)
    qreal hueDeviation = distr(gen) / 2.0; // Hue deviation is usually more sensitive
    hue += hueDeviation;
    if (hue < 0.0) hue += 1.0;
    if (hue > 1.0) hue -= 1.0;

    // Deviate Saturation (clamped)
    qreal satDeviation = distr(gen);
    sat += satDeviation;
    sat = std::max(0.0, std::min(1.0, sat));

    QColor deviatedColor;
    deviatedColor.setHsvF(hue, sat, val, alpha);
    return deviatedColor;
}
