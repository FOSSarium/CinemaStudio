#include "PropertiesPanel.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>

namespace cinemastudio {

PropertiesPanel::PropertiesPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    // Transform
    auto* transformGroup = new QGroupBox("Transform", this);
    auto* transformLayout = new QFormLayout(transformGroup);
    transformLayout->addRow("Position X", new QSpinBox(this));
    transformLayout->addRow("Position Y", new QSpinBox(this));
    transformLayout->addRow("Scale", new QSpinBox(this));
    transformLayout->addRow("Rotation", new QSpinBox(this));
    layout->addWidget(transformGroup);

    // Opacity
    auto* opacityGroup = new QGroupBox("Opacity", this);
    auto* opacityLayout = new QVBoxLayout(opacityGroup);
    auto* opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(100);
    opacityLayout->addWidget(opacitySlider);
    auto* opacityLabel = new QLabel("100%", this);
    opacityLabel->setAlignment(Qt::AlignCenter);
    opacityLabel->setStyleSheet("color: #2196f3; font-size: 14px; font-weight: bold;");
    opacityLayout->addWidget(opacityLabel);
    layout->addWidget(opacityGroup);

    // Info
    auto* infoGroup = new QGroupBox("Clip Info", this);
    auto* infoLayout = new QFormLayout(infoGroup);
    infoLayout->addRow("Name", new QLabel("No clip selected", this));
    infoLayout->addRow("Duration", new QLabel("--", this));
    infoLayout->addRow("Resolution", new QLabel("--", this));
    infoLayout->addRow("Frame Rate", new QLabel("--", this));
    layout->addWidget(infoGroup);

    layout->addStretch();
}

} // namespace cinemastudio
