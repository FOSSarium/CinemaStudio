#pragma once
#include <QWidget>
namespace cinemastudio { class ProjectFile;
class AudioMixer : public QWidget { 
public: AudioMixer(QWidget* parent = nullptr);
void setProject(ProjectFile* project); }; }
