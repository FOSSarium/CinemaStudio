#pragma once

#include "CommonTypes.h"
#include <QLabel>
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
  explicit Timeline(QWidget *parent = nullptr);
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
  void playheadMoved(double time);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void setupUI();
  void drawTimeRuler(QPainter &painter);
  void drawTracks(QPainter &painter);
  void drawPlayhead(QPainter &painter);
  void updateScrollbars();

  ProjectFile *m_project = nullptr;
  EditMode m_editMode = EditMode::Select;

  double m_playheadPosition = 0.0;
  double m_zoom = 50.0; // pixels per second
  int m_trackHeight = 60;
  int m_rulerHeight = 32;

  bool m_draggingPlayhead = false;

  // UI
  QScrollBar *m_hScrollbar = nullptr;
  QScrollBar *m_vScrollbar = nullptr;
  QLabel *m_zoomLabel = nullptr;
  QLabel *m_infoLabel = nullptr;
};

} // namespace cinemastudio
