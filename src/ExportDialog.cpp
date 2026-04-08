#include "ExportDialog.h"
#include "core/ProjectFile.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

namespace cinemastudio {

ExportDialog::ExportDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Export Video");
    resize(500, 400);
    
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    auto* formatGroup = new QGroupBox("Format", this);
    auto* formatLayout = new QFormLayout(formatGroup);
    auto* formatCombo = new QComboBox(this);
    formatCombo->addItems({"MP4 (H.264/AAC)", "AVI (H.264/MP3)", "MOV (H.264/AAC)", "MKV (H.265/AAC)", "WebM (VP9/Opus)"});
    formatLayout->addRow("Container", formatCombo);
    layout->addWidget(formatGroup);

    auto* qualityGroup = new QGroupBox("Quality", this);
    auto* qualityLayout = new QFormLayout(qualityGroup);
    auto* qualityCombo = new QComboBox(this);
    qualityCombo->addItems({"Low (Fast)", "Medium", "High (Recommended)", "Ultra (Slow)", "Custom"});
    qualityCombo->setCurrentIndex(2);
    qualityLayout->addRow("Preset", qualityCombo);
    
    auto* resCombo = new QComboBox(this);
    resCombo->addItems({"1920x1080 (Full HD)", "1280x720 (HD)", "3840x2160 (4K)", "640x480 (SD)"});
    qualityLayout->addRow("Resolution", resCombo);
    
    auto* fpsBox = new QSpinBox(this);
    fpsBox->setRange(1, 120);
    fpsBox->setValue(30);
    qualityLayout->addRow("Frame Rate", fpsBox);
    layout->addWidget(qualityGroup);

    auto* destGroup = new QGroupBox("Destination", this);
    auto* destLayout = new QVBoxLayout(destGroup);
    destLayout->addWidget(new QLabel("Output: ~/Videos/CinemaStudio_export.mp4", this));
    layout->addWidget(destGroup);

    auto* btnLayout = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    auto* exportBtn = new QPushButton("🎬 Export", this);
    exportBtn->setProperty("primary", true);
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(exportBtn);
    layout->addLayout(btnLayout);
    
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(exportBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void ExportDialog::setProject(ProjectFile*) {}

} // namespace cinemastudio
