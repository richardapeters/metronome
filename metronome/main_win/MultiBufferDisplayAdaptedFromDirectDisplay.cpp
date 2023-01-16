#include "metronome/main_win/MultiBufferDisplayAdaptedFromDirectDisplay.hpp"
#include "infra/event/EventDispatcher.hpp"

namespace services
{
    MultiBufferDisplayAdaptedFromDirectDisplay::MultiBufferDisplayAdaptedFromDirectDisplay(hal::DirectDisplay& display)
        : display(display)
    {}

    void MultiBufferDisplayAdaptedFromDirectDisplay::SetBitmap(const infra::Bitmap& bitmap, const infra::Function<void()>& onDone)
    {
        display.DrawBitmap(infra::Point(), bitmap, infra::Region(infra::Point(), display.Size()));
        display.PaintingComplete();
        infra::EventDispatcher::Instance().Schedule(onDone);
    }
}
