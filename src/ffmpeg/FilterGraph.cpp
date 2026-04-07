#include "FilterGraph.h"
#include <QStringList>

namespace cinemastudio {

FilterGraph::FilterGraph() = default;
FilterGraph::~FilterGraph() = default;

QString FilterGraph::buildVideoFilterString(const QStringList& filters) {
    return filters.join(',');
}

QString FilterGraph::buildAudioFilterString(const QStringList& filters) {
    return filters.join(',');
}

QString FilterGraph::buildTransitionFilter(const QString& type, double duration) {
    if (type == "fade") {
        return QString("fade=t=in:st=0:d=%1").arg(duration);
    } else if (type == "fadeout") {
        return QString("fade=t=out:st=0:d=%1").arg(duration);
    } else if (type == "wipeleft") {
        return QString("wipeleft=duration=%1").arg(duration);
    } else if (type == "wiperight") {
        return QString("wiperight=duration=%1").arg(duration);
    } else if (type == "slideup") {
        return QString("slideup=duration=%1").arg(duration);
    } else if (type == "slidedown") {
        return QString("slidedown=duration=%1").arg(duration);
    } else if (type == "dissolve") {
        return QString("dissolve=duration=%1").arg(duration);
    }
    return QString();
}

QString FilterGraph::blur(float radius) {
    return QString("boxblur=%1:1").arg(radius);
}

QString FilterGraph::sharpen(float amount) {
    return QString("unsharp=5:5:%1:5:5:0").arg(amount / 10.0f);
}

QString FilterGraph::brightness(float value) {
    return QString("eq=brightness=%1").arg(value);
}

QString FilterGraph::contrast(float value) {
    return QString("eq=contrast=%1").arg(value);
}

QString FilterGraph::saturation(float value) {
    return QString("eq=saturation=%1").arg(value);
}

QString FilterGraph::hue(float h, float s) {
    return QString("hue=h=%1:s=%2").arg(h).arg(s);
}

QString FilterGraph::vignette(float angle) {
    return QString("vignette=angle=%1").arg(angle);
}

QString FilterGraph::sepia() {
    return "colorchannelmixer=.393:.769:.189:0:.349:.686:.168:0:.272:.534:.131";
}

QString FilterGraph::grayscale() {
    return "hue=s=0";
}

QString FilterGraph::invert() {
    return "negate";
}

QString FilterGraph::scale(int width, int height) {
    return QString("scale=%1:%2").arg(width).arg(height);
}

QString FilterGraph::audioVolume(float volume) {
    return QString("volume=%1").arg(volume);
}

QString FilterGraph::audioFadeIn(double duration) {
    return QString("afade=t=in:st=0:d=%1").arg(duration);
}

QString FilterGraph::audioFadeOut(double duration) {
    return QString("afade=t=out:st=0:d=%1").arg(duration);
}

QString FilterGraph::audioNormalize() {
    return "loudnorm";
}

QString FilterGraph::audioEqualizer(float low, float mid, float high) {
    return QString("equalizer=f=100:t=h:w=100:g=%1,equalizer=f=1000:t=h:w=100:g=%2,equalizer=f=3000:t=h:w=100:g=%3")
        .arg(low).arg(mid).arg(high);
}

} // namespace cinemastudio
