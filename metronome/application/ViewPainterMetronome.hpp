#ifndef METRONOME_VIEW_PAINTER_METRONOME_HPP
#define METRONOME_VIEW_PAINTER_METRONOME_HPP

#include "metronome/interfaces/MetronomePainter.hpp"
#include "preview/interfaces/BitmapPainter.hpp"
#include "preview/interfaces/DoubleBufferDisplay.hpp"
#include "preview/interfaces/View.hpp"

namespace application
{
    class ViewPainterMetronome
        : public services::View
        , public MetronomePainter
    {
    public:
        ViewPainterMetronome(hal::DoubleBufferDisplay& display, hal::BitmapPainter& bitmapPainter, services::View& view, MetronomePainterSubject& subject);

        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;

        virtual void StopAutomaticPainting() override;
        virtual void StartAutomaticPainting() override;
        virtual void ManualPaint() override;
        virtual void SwapLayers(infra::Function<void()> onDone) override;

    protected:
        virtual void Dirty(infra::Region region) override;

    private:
        void Paint();
        void PaintDone();

    private:
        hal::DoubleBufferDisplay& display;
        hal::BitmapPainter& bitmapPainter;
        services::View& view;

        infra::Region dirtyRegion;
        bool scheduled = false;
        bool automaticPainting = true;
    };
}

#endif
