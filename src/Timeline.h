#pragma once

#include "CommonTypes.h"
#include <QScrollBar>
#include <QWidget>
#include <memory>
#include <vector>

namespace cinemastudio {

class ProjectFile;
class TimelineTrack;
class TimelineClip;

class Timeline : public QWidget {
  Q_OBJECT

public:
  Timeline(QWidget *parent = nullptr);
  ~Timeline();

  void setProject(ProjectFile *project);
  void setEditMode(EditMode mode);

  void zoomIn();
  void zoomOut();
  void resetZoom();
  double getZoom() const { return m_zoom; }

  void splitClipAtPlayhead();
  double getPlayheadPosition() const { return m_playheadPosition; }
  void setPlayheadPosition(double time);

  double pixelsToTime(int x) const;
  int timeToPixels(double time) const;

signals:
  void timeChanged(double time);
  void clipSelected(const QString &clipId);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void drawTimeline(QPainter &painter);
  void drawTracks(QPainter &painter);
  void drawPlayhead(QPainter &painter);
  void drawTimeRuler(QPainter &painter);
  void updateScrollbars();
  TimelineTrack *getTrackAtY(int y) const;
  TimelineClip *getClipAtPosition(double time, int trackIndex);

  ProjectFile *m_project = nullptr;
  EditMode m_editMode = EditMode::Select;

  std::vector<std::unique_ptr<TimelineTrack>> m_trackWidgets;

  double m_playheadPosition = 0.0;
  double m_zoom = 50.0; // pixels per second
  int m_trackHeight = 80;
  int m_rulerHeight = 30;

  bool m_draggingPlayhead = false;
  bool m_draggingClip = false;
  QString m_draggedClipId;
  double m_dragStartTime = 0.0;

  QScrollBar *m_hScrollbar = nullptr;
  QScrollBar *m_vScrollbar = nullptr;
};

} // namespace cinemastudio
