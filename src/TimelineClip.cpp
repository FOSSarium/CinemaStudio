#include "TimelineClip.h"
#include "core/Clip.h"
namespace cinemastudio {
TimelineClip::TimelineClip(Clip* clip, QWidget* parent) : QWidget(parent), m_clip(clip) {}
}
