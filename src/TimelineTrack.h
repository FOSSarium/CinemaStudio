#pragma once
#include <QWidget>
namespace cinemastudio { class ProjectFile; class Timeline;
class TimelineTrack : public QWidget { 
public: TimelineTrack(int index, QWidget* parent = nullptr);
int getIndex() const { return m_index; }
private: int m_index; }; }
