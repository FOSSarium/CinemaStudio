#include "AudioMixer.h"
#include "core/ProjectFile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QGroupBox>

namespace cinemastudio {

AudioMixer::AudioMixer(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);
    
    auto* mixerLabel = new QLabel("🔊 Audio Mixer", this);
    mixerLabel->setStyleSheet("color: #e0e0e0; font-size: 12px; font-weight: bold;");
    layout->addWidget(mixerLabel);

    auto* mixerLayout = new QHBoxLayout();
    mixerLayout->setSpacing(16);
    
    // Master
    auto* masterGroup = new QGroupBox("Master", this);
    auto* masterLayout = new QVBoxLayout(masterGroup);
    auto* masterSlider = new QSlider(Qt::Vertical, this);
    masterSlider->setRange(0, 100);
    masterSlider->setValue(80);
    masterSlider->setFixedHeight(60);
    masterLayout->addWidget(masterSlider);
    masterLayout->addWidget(new QLabel("80%", this));
    mixerLayout->addWidget(masterGroup);

    // Track 1
    auto* t1Group = new QGroupBox("Video 1", this);
    auto* t1Layout = new QVBoxLayout(t1Group);
    auto* t1Slider = new QSlider(Qt::Vertical, this);
    t1Slider->setRange(0, 100);
    t1Slider->setValue(100);
    t1Slider->setFixedHeight(60);
    t1Layout->addWidget(t1Slider);
    t1Layout->addWidget(new QLabel("100%", this));
    mixerLayout->addWidget(t1Group);

    // Track 2
    auto* t2Group = new QGroupBox("Audio 1", this);
    auto* t2Layout = new QVBoxLayout(t2Group);
    auto* t2Slider = new QSlider(Qt::Vertical, this);
    t2Slider->setRange(0, 100);
    t2Slider->setValue(100);
    t2Slider->setFixedHeight(60);
    t2Layout->addWidget(t2Slider);
    t2Layout->addWidget(new QLabel("100%", this));
    mixerLayout->addWidget(t2Group);
    
    mixerLayout->addStretch();
    layout->addLayout(mixerLayout);
}

void AudioMixer::setProject(ProjectFile*) {}

} // namespace cinemastudio
