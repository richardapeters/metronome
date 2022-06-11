#include "infra/event/EventDispatcher.hpp"
#include "metronome/views/ViewPainterMetronome.hpp"
#include "preview/interfaces/BitmapCanvas.hpp"

namespace application
{
    ViewPainterMetronome::ViewPainterMetronome(hal::DoubleBufferDisplay& display, hal::BitmapPainter& bitmapPainter, services::View& view, MetronomePainterSubject& subject)
        : MetronomePainter(subject)
        , display(display)
        , bitmapPainter(bitmapPainter)
        , view(view)
    {
        view.SetParent(*this);
        Dirty(view.ViewRegion());
    }

    void ViewPainterMetronome::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        std::abort();
    }

    void ViewPainterMetronome::StopAutomaticPainting()
    {
        automaticPainting = false;
    }

    void ViewPainterMetronome::StartAutomaticPainting()
    {
        automaticPainting = true;
        Dirty(infra::Region());
    }

    void ViewPainterMetronome::ManualPaint()
    {
        services::BitmapCanvas canvas(display.DrawingBitmap(), bitmapPainter);
        view.Paint(canvas, view.ViewRegion());
        bitmapPainter.WaitUntilDrawingFinished();
    }

    void ViewPainterMetronome::SwapLayers(infra::Function<void()> onDone)
    {
        display.SwapLayers(onDone);
    }

    void ViewPainterMetronome::Dirty(infra::Region region)
    {
        dirtyRegion = dirtyRegion | region;

        if (automaticPainting && !scheduled && !dirtyRegion.Empty())
        {
            dirtyRegion = infra::Region();
            scheduled = true;
            infra::EventDispatcher::Instance().Schedule([this]()
                {
                    if (automaticPainting)
                        Paint();
                    else
                        scheduled = false;
                });
        }
    }

    void ViewPainterMetronome::Paint()
    {
        ManualPaint();

        SwapLayers([this]() { PaintDone(); });
    }

    void ViewPainterMetronome::PaintDone()
    {
        scheduled = false;
        Dirty(dirtyRegion);
    }
}
