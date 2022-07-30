#include "infra/event/EventDispatcher.hpp"
#include "metronome/application/BeatPainter.hpp"

namespace application
{
    BeatPainter::BeatPainter(hal::MultiBufferDisplay& display, BeatTimer& subject, hal::BitmapPainter& painter, infra::Bitmap& bitmap0, infra::Bitmap& bitmap1, infra::Bitmap& beatBitmap)
        : BeatTimerObserver(subject)
        , display(display)
        , drawingBitmap(&bitmap0)
        , viewingBitmap(&bitmap1)
        , beatBitmap(beatBitmap)
    {
        auto bitmapRegion = infra::Region(infra::Point(), beatBitmap.size);
        painter.DrawFilledRectangle(beatBitmap, bitmapRegion, infra::Colour::red, bitmapRegion);
    }

    void BeatPainter::SwapLayers(const infra::Function<void()>& onDone)
    {
        std::swap(drawingBitmap, viewingBitmap);
        onSwapDone = onDone;

        EvaluateSetBitmap();
    }

    infra::Bitmap& BeatPainter::DrawingBitmap()
    {
        return *drawingBitmap;
    }

    const infra::Bitmap& BeatPainter::ViewingBitmap() const
    {
        return *viewingBitmap;
    }

    void BeatPainter::Beat()
    {
        // Invoked on interrupt context

        beatOn = true;
        beatOnOffRequested = true;
        EvaluateSetBitmap();

        infra::EventDispatcher::Instance().Schedule([this]()
            {
                beatOff.Start(std::chrono::milliseconds(30), [this]()
                    {
                        beatOn = false;
                        beatOnOffRequested = true;

                        if (onSwapDone != nullptr)
                            onBeatOffDone = std::move(onSwapDone);

                        EvaluateSetBitmap();
                    });
            });
    }

    void BeatPainter::Started(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure)
    {}

    void BeatPainter::Stopped()
    {}

    void BeatPainter::EvaluateSetBitmap()
    {
        if (beatOnOffRequested)
        {
            if (!settingBitmap.test_and_set())
            {
                if (beatOn)
                    display.SetBitmap(beatBitmap, [this]()
                        {
                            settingBitmap.clear();
                            beatOnOffRequested = false;
                            EvaluateSetBitmap();
                        });
                else
                    display.SetBitmap(*viewingBitmap, [this]()
                        {
                            settingBitmap.clear();
                            beatOnOffRequested = false;
                            if (onBeatOffDone)
                                onBeatOffDone();
                            EvaluateSetBitmap();
                        });
            }
        }
        else if (!beatOn && onSwapDone != nullptr)
        {
            if (!settingBitmap.test_and_set())
                display.SetBitmap(*viewingBitmap, [this]()
                    {
                        settingBitmap.clear();
                        onSwapDone();
                        EvaluateSetBitmap();
                    });
        }
    }
}
