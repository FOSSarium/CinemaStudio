#pragma once
#include <QWidget>
namespace cinemastudio { class Clip;
class TimelineClip : public QWidget { 
public: TimelineClip(Clip* clip, QWidget* parent = nullptr);
private: Clip* m_clip; }; }
