#ifndef METRONOME_MULTI_BUFFER_DISPLAY_ADAPTED_FROM_DIRECT_DISPLAY
#define METRONOME_MULTI_BUFFER_DISPLAY_ADAPTED_FROM_DIRECT_DISPLAY

#include "preview/interfaces/DirectDisplay.hpp"
#include "preview/interfaces/MultiBufferDisplay.hpp"

namespace services
{
    class MultiBufferDisplayAdaptedFromDirectDisplay
        : public hal::MultiBufferDisplay
    {
    public:
        MultiBufferDisplayAdaptedFromDirectDisplay(hal::DirectDisplay& display);

        virtual void SetBitmap(const infra::Bitmap& bitmap, const infra::Function<void()>& onDone) override;

    private:
        hal::DirectDisplay& display;
    };
}

#endif
