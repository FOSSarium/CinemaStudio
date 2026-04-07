#pragma once

#include <QString>
#include <QStringList>

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}

namespace cinemastudio {

class FilterGraph {
public:
    FilterGraph();
    ~FilterGraph();

    // Build filter graph string
    static QString buildVideoFilterString(const QStringList& filters);
    static QString buildAudioFilterString(const QStringList& filters);
    static QString buildTransitionFilter(const QString& type, double duration);
    
    // Common filters
    static QString blur(float radius);
    static QString sharpen(float amount);
    static QString brightness(float value);
    static QString contrast(float value);
    static QString saturation(float value);
    static QString hue(float h, float s);
    static QString vignette(float angle);
    static QString sepia();
    static QString grayscale();
    static QString invert();
    static QString scale(int width, int height);
    
    // Audio filters
    static QString audioVolume(float volume);
    static QString audioFadeIn(double duration);
    static QString audioFadeOut(double duration);
    static QString audioNormalize();
    static QString audioEqualizer(float low, float mid, float high);

private:
    AVFilterGraph* m_filterGraph = nullptr;
    AVFilterContext* m_buffersinkCtx = nullptr;
    AVFilterContext* m_buffersrcCtx = nullptr;
};

} // namespace cinemastudio
