#include "metronome/application/BeatPainter.hpp"
#include "infra/event/EventDispatcher.hpp"

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

    void BeatPainter::Beat(uint8_t subDivision, bool gapped)
    {
        // Invoked on interrupt context

        if (!gapped && subDivision % 12 == 0)
        {
            beatOn = true;

            if (!settingBitmap.test_and_set())
            {
                display.SetBitmap(beatBitmap, [this]()
                    {
                        settingBitmap.clear();
                        EvaluateSetBitmap(); });
            }
            else
                beatOnOffRequested = true;

            infra::EventDispatcher::Instance().Schedule([this]()
                { beatOff.Start(std::chrono::milliseconds(30), [this]()
                      {
                            if (!settingBitmap.test_and_set() && onSwapDone != nullptr)
                            {
                                onBeatOffDone = std::move(onSwapDone);
                                settingBitmap.clear();
                            }

                            beatOn = false;
                            beatOnOffRequested = true;

                            EvaluateSetBitmap(); }); });
        }
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
                beatOnOffRequested = false;
                if (beatOn)
                    display.SetBitmap(beatBitmap, [this]()
                        {
                            settingBitmap.clear();
                            EvaluateSetBitmap(); });
                else
                    display.SetBitmap(*viewingBitmap, [this]()
                        {
                            if (onBeatOffDone)
                                onBeatOffDone();
                            settingBitmap.clear();
                            EvaluateSetBitmap(); });
            }
        }
        else if (!beatOn && onSwapDone != nullptr)
        {
            if (!settingBitmap.test_and_set())
                display.SetBitmap(*viewingBitmap, [this]()
                    {
                        onSwapDone();
                        settingBitmap.clear();
                        EvaluateSetBitmap(); });
        }
    }
}
