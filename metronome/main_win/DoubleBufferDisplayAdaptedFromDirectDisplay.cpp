#include "metronome/main_win/DoubleBufferDisplayAdaptedFromDirectDisplay.hpp"

namespace services
{
    DoubleBufferDisplayAdaptedFromDirectDisplay::DoubleBufferDisplayAdaptedFromDirectDisplay(infra::Bitmap& drawingBitmap, infra::Bitmap& viewingBitmap, hal::DirectDisplay& display)
        : drawingBitmap(&drawingBitmap)
        , viewingBitmap(&viewingBitmap)
        , display(display)
    {}

    void DoubleBufferDisplayAdaptedFromDirectDisplay::SwapLayers(const infra::Function<void()>& onDone)
    {
        std::swap(drawingBitmap, viewingBitmap);
        display.DrawBitmap(infra::Point(), *viewingBitmap, infra::Region(infra::Point(), display.Size()));
        display.PaintingComplete();
        onDone();
    }

    infra::Bitmap& DoubleBufferDisplayAdaptedFromDirectDisplay::DrawingBitmap()
    {
        return *drawingBitmap;
    }

    const infra::Bitmap& DoubleBufferDisplayAdaptedFromDirectDisplay::ViewingBitmap() const
    {
        return *viewingBitmap;
    }
}
