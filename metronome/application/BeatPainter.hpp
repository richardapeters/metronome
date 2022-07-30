#ifndef METRONOME_BEAT_PAINTER_HPP
#define METRONOME_BEAT_PAINTER_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "metronome/interfaces/BeatTimer.hpp"
#include "preview/interfaces/BitmapPainter.hpp"
#include "preview/interfaces/DoubleBufferDisplay.hpp"
#include "preview/interfaces/MultiBufferDisplay.hpp"
#include <atomic>

namespace application
{
    class BeatPainter
        : public hal::DoubleBufferDisplay
        , public BeatTimerObserver
    {
    public:
        BeatPainter(hal::MultiBufferDisplay& display, BeatTimer& subject, hal::BitmapPainter& painter, infra::Bitmap& bitmap0, infra::Bitmap& bitmap1, infra::Bitmap& beatBitmap);

        // Implementation of DoubleBufferDisplay
        virtual void SwapLayers(const infra::Function<void()>& onDone) override;
        virtual infra::Bitmap& DrawingBitmap() override;
        virtual const infra::Bitmap& ViewingBitmap() const override;

        // Implementation of BeatTimer
        virtual void Beat() override;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure) override;
        virtual void Stopped() override;

    private:
        void EvaluateSetBitmap();

    private:
        hal::MultiBufferDisplay& display;

        infra::Bitmap* drawingBitmap;
        infra::Bitmap* viewingBitmap;
        infra::Bitmap& beatBitmap;

        std::atomic_flag settingBitmap{ ATOMIC_FLAG_INIT };
        std::atomic<bool> beatOn{ false };
        std::atomic<bool> beatOnOffRequested{ false };
        infra::TimerSingleShot beatOff;

        infra::AutoResetFunction<void()> onSwapDone;
        infra::AutoResetFunction<void()> onBeatOffDone;
    };
}

#endif
