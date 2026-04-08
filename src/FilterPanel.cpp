#include "FilterPanel.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QFormLayout>

namespace cinemastudio {

FilterPanel::FilterPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    auto* colorGroup = new QGroupBox("🎨 Color Correction", this);
    auto* colorLayout = new QFormLayout(colorGroup);
    
    auto createSlider = [&]() {
        auto* s = new QSlider(Qt::Horizontal, this);
        s->setRange(-100, 100);
        s->setValue(0);
        return s;
    };
    
    colorLayout->addRow("Brightness", createSlider());
    colorLayout->addRow("Contrast", createSlider());
    colorLayout->addRow("Saturation", createSlider());
    colorLayout->addRow("Temperature", createSlider());
    layout->addWidget(colorGroup);

    auto* detailGroup = new QGroupBox("✨ Detail", this);
    auto* detailLayout = new QFormLayout(detailGroup);
    detailLayout->addRow("Sharpness", createSlider());
    detailLayout->addRow("Blur", createSlider());
    layout->addWidget(detailGroup);

    auto* lightGroup = new QGroupBox("💡 Light", this);
    auto* lightLayout = new QFormLayout(lightGroup);
    lightLayout->addRow("Exposure", createSlider());
    lightLayout->addRow("Highlights", createSlider());
    lightLayout->addRow("Shadows", createSlider());
    layout->addWidget(lightGroup);

    layout->addStretch();
}

} // namespace cinemastudio
