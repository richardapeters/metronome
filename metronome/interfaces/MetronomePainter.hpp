#ifndef METRONOME_METRONOME_PAINTER_HPP
#define METRONOME_METRONOME_PAINTER_HPP

#include "infra/util/Function.hpp"
#include "infra/util/Observer.hpp"

namespace application
{
    class MetronomePainterSubject;

    class MetronomePainter
        : public infra::SingleObserver<MetronomePainter, MetronomePainterSubject>
    {
    public:
        using infra::SingleObserver<MetronomePainter, MetronomePainterSubject>::SingleObserver;

        virtual void StopAutomaticPainting() = 0;
        virtual void StartAutomaticPainting() = 0;
        virtual void ManualPaint() = 0;
        virtual void SwapLayers(infra::Function<void()> onDone) = 0;
    };

    class MetronomePainterSubject
        : public infra::Subject<MetronomePainter>
    {};
}

#endif
