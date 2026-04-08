#include "Timeline.h"
#include "core/Clip.h"
#include "core/ProjectFile.h"
#include "core/Track.h"
#include <QApplication>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWheelEvent>

namespace cinemastudio {

Timeline::Timeline(QWidget *parent) : QWidget(parent) {
  setObjectName("timeline");
  setupUI();
}

Timeline::~Timeline() = default;

void Timeline::setupUI() {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  m_hScrollbar = new QScrollBar(Qt::Horizontal, this);
  m_vScrollbar = new QScrollBar(Qt::Vertical, this);

  connect(m_hScrollbar, &QScrollBar::valueChanged, this,
          [this]() { update(); });
  connect(m_vScrollbar, &QScrollBar::valueChanged, this,
          [this]() { update(); });

  // Info bar
  auto *infoLayout = new QHBoxLayout();
  infoLayout->setContentsMargins(10, 4, 10, 4);

  m_infoLabel = new QLabel("Timeline", this);
  m_infoLabel->setStyleSheet(
      "color: #888888; font-size: 11px; padding-left: 8px;");
  infoLayout->addWidget(m_infoLabel);
  infoLayout->addStretch();

  m_zoomLabel = new QLabel("100%", this);
  m_zoomLabel->setStyleSheet(
      "color: #6366F1; font-size: 11px; padding-right: 8px;");
  infoLayout->addWidget(m_zoomLabel);

  layout->addLayout(infoLayout);
}

void Timeline::setProject(ProjectFile *project) {
  m_project = project;
  if (project) {
    m_infoLabel->setText(
        QString("Timeline — %1 tracks").arg(project->getTrackCount()));
  }
  update();
}

void Timeline::setEditMode(EditMode mode) {
  m_editMode = mode;
  QString modeStr;
  switch (mode) {
  case EditMode::Select:
    modeStr = "Select";
    break;
  case EditMode::Cut:
    modeStr = "Cut";
    break;
  case EditMode::Trim:
    modeStr = "Trim";
    break;
  case EditMode::Split:
    modeStr = "Split";
    break;
  case EditMode::Hand:
    modeStr = "Hand";
    break;
  }
  m_infoLabel->setText(QString("Timeline — Mode: %1").arg(modeStr));
}

void Timeline::zoomIn() {
  m_zoom = std::min(m_zoom * 1.3, 500.0);
  m_zoomLabel->setText(QString::number((int)(m_zoom / 50.0 * 100)) + "%");
  updateScrollbars();
  update();
}

void Timeline::zoomOut() {
  m_zoom = std::max(m_zoom / 1.3, 10.0);
  m_zoomLabel->setText(QString::number((int)(m_zoom / 50.0 * 100)) + "%");
  updateScrollbars();
  update();
}

void Timeline::resetZoom() {
  m_zoom = 50.0;
  m_zoomLabel->setText("100%");
  updateScrollbars();
  update();
}

void Timeline::splitClipAtPlayhead() {
  if (!m_project)
    return;

  auto tracks = m_project->getVideoTracks();
  for (auto track : tracks) {
    auto clip = track->getClipAtTime(m_playheadPosition);
    if (clip && !clip->getFilePath().isEmpty()) {
      double splitTime =
          m_playheadPosition - clip->getProperties().timelinePosition;
      Clip newClip;
      clip->split(clip->getProperties().startTime + splitTime, newClip);
      track->addClip(std::make_unique<Clip>(newClip));
      update();
      return;
    }
  }
}

void Timeline::setPlayheadPosition(double time) {
  m_playheadPosition = std::max(0.0, time);
  emit timeChanged(m_playheadPosition);
  emit playheadMoved(m_playheadPosition);
  update();
}

double Timeline::pixelsToTime(int x) const {
  return (x + m_hScrollbar->value()) / m_zoom;
}

int Timeline::timeToPixels(double time) const {
  return (int)(time * m_zoom) - m_hScrollbar->value();
}

void Timeline::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int contentTop = m_rulerHeight + 16; // Account for info bar

  // Background
  painter.fillRect(rect(), QColor("#151518"));

  drawTimeRuler(painter);
  drawTracks(painter);
  drawPlayhead(painter);
}

void Timeline::drawTimeRuler(QPainter &painter) {
  int rulerTop = m_rulerHeight + 8;
  painter.fillRect(8, rulerTop, width() - 16, m_rulerHeight - 8,
                   QColor("#1A1A1E"));

  painter.setPen(QColor("#e0e0e0"));
  painter.setFont(QFont("Monospace", 9));

  double timeStep = 1.0;
  if (m_zoom < 20)
    timeStep = 5.0;
  if (m_zoom < 10)
    timeStep = 10.0;
  if (m_zoom > 100)
    timeStep = 0.5;
  if (m_zoom > 200)
    timeStep = 0.25;

  int start_t = m_hScrollbar->value() / m_zoom;
  int end_t = (m_hScrollbar->value() + width()) / m_zoom + 1;

  for (int t = start_t; t <= end_t; t++) {
    int x = timeToPixels(t) + 8;
    if (x < 8 || x > width() - 8)
      continue;

    bool isMajor = (t % (int)timeStep == 0);
    painter.setPen(isMajor ? QColor("#e0e0e0") : QColor("#666666"));
    painter.drawLine(x, rulerTop + (isMajor ? 12 : 18), x,
                     rulerTop + m_rulerHeight - 8);

    if (isMajor) {
      painter.drawText(x + 3, rulerTop + 14, QString::number(t) + "s");
    }
  }
}

void Timeline::drawTracks(QPainter &painter) {
  if (!m_project)
    return;

  int trackStartY = m_rulerHeight + 16;
  int trackCount = m_project->getTrackCount();
  int totalHeight = trackCount * m_trackHeight;

  m_vScrollbar->setRange(0, std::max(0, totalHeight - height() + trackStartY));
  m_vScrollbar->setPageStep(height() - trackStartY);

  for (int i = 0; i < trackCount; i++) {
    auto track = m_project->getTrack(i);
    if (!track)
      continue;

    int trackY = trackStartY + i * m_trackHeight - m_vScrollbar->value();
    if (trackY + m_trackHeight < trackStartY || trackY > height())
      continue;

    // Track background
    QColor bgColor = (i % 2 == 0) ? QColor("#1A1A1E") : QColor("#151518");
    painter.fillRect(8, trackY, width() - 16, m_trackHeight - 2, bgColor);

    // Track header
    QString trackType;
    QColor trackColor;
    switch (track->getType()) {
    case TrackType::Video:
      trackType = "🎬";
      trackColor = QColor("#6366F1"); // Movavi-style purple/blue
      break;
    case TrackType::Audio:
      trackType = "🔊";
      trackColor = QColor("#10B981"); // Modern emerald
      break;
    case TrackType::Text:
      trackType = "📝";
      trackColor = QColor("#F59E0B"); // Modern amber
      break;
    }

    painter.fillRect(8, trackY, 120, m_trackHeight - 2, trackColor.darker(150));
    painter.setPen(trackColor);
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(16, trackY + 20, trackType);
    painter.setPen(QColor("#e0e0e0"));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(40, trackY + 20, track->getName());

    // Visibility/Lock indicators
    if (!track->isVisible()) {
      painter.setPen(QColor("#f44336"));
      painter.drawText(16, trackY + 36, "👁");
    }
    if (track->isLocked()) {
      painter.setPen(QColor("#ff9800"));
      painter.drawText(100, trackY + 36, "🔒");
    }

    // Track separator line
    painter.setPen(QColor("#2E2E38"));
    painter.drawLine(8, trackY + m_trackHeight - 2, width() - 8,
                     trackY + m_trackHeight - 2);

    // Draw clips
    for (int j = 0; j < track->getClipCount(); j++) {
      auto clip = track->getClipAt(j);
      if (!clip)
        continue;

      int clipX = timeToPixels(clip->getProperties().timelinePosition) + 8;
      int clipW = (int)(clip->getDuration() * m_zoom);

      if (clipX + clipW < 8 || clipX > width() - 8)
        continue;

      // Clip background - Use rounded rect directly for fill
      QColor clipColor = trackColor;
      QColor clipFillColor = clipColor.lighter(120);
      clipFillColor.setAlpha(200); // Slight transparency
      
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(Qt::NoPen);
      painter.setBrush(clipFillColor);
      painter.drawRoundedRect(clipX + 2, trackY + 26, clipW - 4, m_trackHeight - 30, 6, 6);

      // Clip border
      painter.setPen(QPen(clipColor, 1));
      painter.setBrush(Qt::NoBrush);
      painter.drawRoundedRect(clipX + 2, trackY + 26, clipW - 4,
                              m_trackHeight - 30, 6, 6);

      // Clip name
      painter.setPen(QColor("#ffffff"));
      painter.setFont(QFont("Arial", 9, QFont::Bold));
      QFontMetrics fm(painter.font());
      QString clipName =
          fm.elidedText(clip->getName(), Qt::ElideRight, clipW - 12);
      painter.drawText(clipX + 8, trackY + 42, clipName);

      // Clip duration
      painter.setPen(QColor("#cccccc"));
      painter.setFont(QFont("Arial", 8));
      QString durStr = QString::number(clip->getDuration(), 'f', 1) + "s";
      painter.drawText(clipX + 8, trackY + 54, durStr);

      // Thumbnail if available
      if (!clip->getThumbnail().isNull() && clipW > 60) {
        QImage thumb = clip->getThumbnail().scaled(
            clipW - 12, m_trackHeight - 40, Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
        painter.drawImage(clipX + 6, trackY + 28, thumb);
      }
    }
  }
}

void Timeline::drawPlayhead(QPainter &painter) {
  int x = timeToPixels(m_playheadPosition) + 8;
  if (x < 8 || x > width() - 8)
    return;

  int trackStartY = m_rulerHeight + 8;

  // Playhead line
  QColor playheadColor = QColor("#F43F5E"); // Rose color
  painter.setPen(QPen(playheadColor, 2));
  painter.drawLine(x, trackStartY, x, height());

  // Playhead marker (Sleek triangle)
  QPolygon triangle;
  triangle << QPoint(x - 8, trackStartY - 4) << QPoint(x + 8, trackStartY - 4)
           << QPoint(x, trackStartY + 10);
  painter.setBrush(playheadColor);
  painter.setPen(Qt::NoPen);
  painter.drawPolygon(triangle);
}

void Timeline::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    int rulerTop = m_rulerHeight + 8;
    if (event->pos().y() >= rulerTop &&
        event->pos().y() <= rulerTop + m_rulerHeight) {
      m_draggingPlayhead = true;
      setPlayheadPosition(pixelsToTime(event->pos().x() - 8));
    }
  }
}

void Timeline::mouseMoveEvent(QMouseEvent *event) {
  if (m_draggingPlayhead) {
    setPlayheadPosition(pixelsToTime(event->pos().x() - 8));
  }
}

void Timeline::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_draggingPlayhead = false;
  }
}

void Timeline::wheelEvent(QWheelEvent *event) {
  if (event->modifiers() & Qt::ControlModifier) {
    if (event->angleDelta().y() > 0)
      zoomIn();
    else
      zoomOut();
  } else {
    m_hScrollbar->setValue(m_hScrollbar->value() - event->angleDelta().y() / 8);
  }
}

void Timeline::resizeEvent(QResizeEvent *) { updateScrollbars(); }

void Timeline::updateScrollbars() {
  if (!m_project)
    return;

  double maxTime = std::max(m_project->getTimelineDuration(), 30.0);
  int maxPixels = (int)(maxTime * m_zoom);
  m_hScrollbar->setRange(0, std::max(0, maxPixels - width() + 20));
  m_hScrollbar->setPageStep(width());

  m_hScrollbar->setGeometry(8, height() - 16, width() - 24, 12);
  m_vScrollbar->setGeometry(width() - 16, m_rulerHeight + 16, 12,
                            height() - m_rulerHeight - 28);
}

} // namespace cinemastudio
