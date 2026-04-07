#include "Timeline.h"
#include "MainWindow.h"
#include "TimelineTrack.h"
#include "core/Clip.h"
#include "core/ProjectFile.h"
#include "core/Track.h"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>

namespace cinemastudio {

Timeline::Timeline(QWidget *parent) : QWidget(parent) {
  m_hScrollbar = new QScrollBar(Qt::Horizontal, this);
  m_vScrollbar = new QScrollBar(Qt::Vertical, this);
  connect(m_hScrollbar, &QScrollBar::valueChanged, this,
          [this](int) { update(); });
  connect(m_vScrollbar, &QScrollBar::valueChanged, this,
          [this](int) { update(); });
}

Timeline::~Timeline() = default;

void Timeline::setProject(ProjectFile *project) {
  m_project = project;
  update();
}

void Timeline::setEditMode(EditMode mode) { m_editMode = mode; }

void Timeline::zoomIn() {
  m_zoom = std::min(m_zoom * 1.2, 500.0);
  update();
}

void Timeline::zoomOut() {
  m_zoom = std::max(m_zoom / 1.2, 10.0);
  update();
}

void Timeline::resetZoom() {
  m_zoom = 50.0;
  update();
}

void Timeline::splitClipAtPlayhead() {
  if (!m_project)
    return;
  // TODO: Implement split logic
}

void Timeline::setPlayheadPosition(double time) {
  m_playheadPosition = std::max(0.0, time);
  emit timeChanged(m_playheadPosition);
  update();
}

double Timeline::pixelsToTime(int x) const { return x / m_zoom; }

int Timeline::timeToPixels(double time) const {
  return static_cast<int>(time * m_zoom);
}

void Timeline::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.fillRect(rect(), QColor("#2b2b2b"));
  drawTimeRuler(painter);
  drawTracks(painter);
  drawPlayhead(painter);
}

void Timeline::drawTimeRuler(QPainter &painter) {
  painter.fillRect(0, 0, width(), m_rulerHeight, QColor("#3b3b3b"));
  painter.setPen(Qt::white);

  double timeStep = 1.0;
  if (m_zoom < 20)
    timeStep = 5.0;
  if (m_zoom < 10)
    timeStep = 10.0;
  if (m_zoom > 100)
    timeStep = 0.5;

  int end_time = static_cast<int>(width() / m_zoom) + 10;
  for (int t = 0; t <= end_time; t += timeStep) {
    int x = timeToPixels(t);
    painter.drawLine(x, m_rulerHeight - 10, x, m_rulerHeight);
    painter.drawText(x + 5, 15, QString::number(t));
  }
}

void Timeline::drawTracks(QPainter &painter) {
  if (!m_project)
    return;

  int y = m_rulerHeight;
  int trackCount = m_project->getTrackCount();

  for (int i = 0; i < trackCount; i++) {
    auto track = m_project->getTrack(i);
    int trackY = y - m_vScrollbar->value();

    // Track background
    QColor bgColor = (i % 2 == 0) ? QColor("#333333") : QColor("#2d2d2d");
    painter.fillRect(0, trackY, width(), m_trackHeight, bgColor);

    // Track label
    painter.setPen(Qt::white);
    painter.drawText(10, trackY + 20, track->getName());

    // Draw clips
    for (int j = 0; j < track->getClipCount(); j++) {
      auto clip = track->getClipAt(j);
      int clipX = timeToPixels(clip->getProperties().timelinePosition);
      int clipW = timeToPixels(clip->getDuration());

      painter.fillRect(clipX, trackY + 5, clipW, m_trackHeight - 10,
                       track->getColor());
      painter.setPen(Qt::white);
      painter.drawText(clipX + 5, trackY + 25, clip->getName());
    }

    y += m_trackHeight;
  }
}

void Timeline::drawPlayhead(QPainter &painter) {
  int x = timeToPixels(m_playheadPosition);
  painter.setPen(QPen(Qt::red, 2));
  painter.drawLine(x, 0, x, height());

  // Playhead triangle
  QPolygon triangle;
  triangle << QPoint(x - 6, 0) << QPoint(x + 6, 0) << QPoint(x, 10);
  painter.setBrush(Qt::red);
  painter.setPen(Qt::NoPen);
  painter.drawPolygon(triangle);
}

void Timeline::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (event->pos().y() < m_rulerHeight) {
      m_draggingPlayhead = true;
      setPlayheadPosition(pixelsToTime(event->pos().x()));
    }
  }
}

void Timeline::mouseMoveEvent(QMouseEvent *event) {
  if (m_draggingPlayhead) {
    setPlayheadPosition(pixelsToTime(event->pos().x()));
  }
}

void Timeline::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_draggingPlayhead = false;
    m_draggingClip = false;
  }
}

void Timeline::wheelEvent(QWheelEvent *event) {
  if (event->modifiers() & Qt::ControlModifier) {
    if (event->angleDelta().y() > 0) {
      zoomIn();
    } else {
      zoomOut();
    }
  } else {
    m_hScrollbar->setValue(m_hScrollbar->value() - event->angleDelta().y() / 8);
  }
}

void Timeline::resizeEvent(QResizeEvent *event) { updateScrollbars(); }

void Timeline::updateScrollbars() {
  m_hScrollbar->setGeometry(0, height() - 20, width() - 20, 20);
  m_vScrollbar->setGeometry(width() - 20, m_rulerHeight, 20,
                            height() - m_rulerHeight - 20);
}

TimelineTrack *Timeline::getTrackAtY(int y) const {
  int trackIndex = (y - m_rulerHeight + m_vScrollbar->value()) / m_trackHeight;
  if (m_project && trackIndex >= 0 && trackIndex < m_project->getTrackCount()) {
    // Return track from project
  }
  return nullptr;
}

TimelineClip *Timeline::getClipAtPosition(double time, int trackIndex) {
  if (!m_project)
    return nullptr;
  auto track = m_project->getTrack(trackIndex);
  if (!track)
    return nullptr;
  return nullptr;
}

} // namespace cinemastudio
