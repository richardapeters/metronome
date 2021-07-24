#ifndef METRONOME_DOUBLE_BUFFER_DISPLAY_ADAPTED_FROM_DIRECT_DISPLAY
#define METRONOME_DOUBLE_BUFFER_DISPLAY_ADAPTED_FROM_DIRECT_DISPLAY

#include "infra/util/WithStorage.hpp"
#include "preview/interfaces/DirectDisplay.hpp"
#include "preview/interfaces/DoubleBufferDisplay.hpp"

namespace services
{
    class DoubleBufferDisplayAdaptedFromDirectDisplay
        : public hal::DoubleBufferDisplay
    {
    public:
        template<int32_t width, int32_t height, infra::PixelFormat pixelFormat>
        using WithStorage = infra::WithStorage<infra::WithStorage<DoubleBufferDisplayAdaptedFromDirectDisplay,
            infra::Bitmap::WithStorage<width, height, pixelFormat>>,
            infra::Bitmap::WithStorage<width, height, pixelFormat>>;

        DoubleBufferDisplayAdaptedFromDirectDisplay(infra::Bitmap& drawingBitmap, infra::Bitmap& viewingBitmap, hal::DirectDisplay& display);

        virtual void SwapLayers(const infra::Function<void()>& onDone) override;
        virtual infra::Bitmap& DrawingBitmap() override;
        virtual const infra::Bitmap& ViewingBitmap() const override;

    private:
        infra::Bitmap* drawingBitmap;
        infra::Bitmap* viewingBitmap;
        hal::DirectDisplay& display;
    };
}

#endif
