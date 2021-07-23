#include "hal/generic/TimerServiceGeneric.hpp"
#include "infra/event/LowPowerEventDispatcher.hpp"
#include "infra/timer/DerivedTimerService.hpp"
#include "infra/util/PostAssign.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/ViewDateEntry.hpp"
#include "preview/sdl/DirectDisplaySdl.hpp"
#include "preview/sdl/LowPowerStrategySdl.hpp"
#include "preview/sdl/SdlTouchInteractor.hpp"
#include <Windows.h>

class MyTimerService
    : public services::SettableTimerService
{
public:
    MyTimerService(uint32_t id, TimerService& baseTimerService)
        : services::SettableTimerService(id)
        , baseTimerService(baseTimerService)
    {
        Shift(std::chrono::hours(2));
    }

    void SetTime(infra::TimePoint time, const infra::Function<void()>& onDone) override
    {
        Shift(time - Now() + GetCurrentShift());
        onDone();
    }

    virtual infra::TimePoint Now() const override
    {
        return baseTimerService.Now() + shift;
    }

    virtual infra::Duration Resolution() const override
    {
        return baseTimerService.Resolution();
    }

    virtual void NextTriggerChanged() override
    {
        nextTrigger = NextTrigger() - shift;

        if (NextTrigger() != infra::TimePoint::max())
            timer.Start(nextTrigger, [this]() { Progressed(nextTrigger + shift); });
        else
            timer.Cancel();
    }

    void Shift(infra::Duration shift)
    {
        auto oldTime = Now();
        this->shift = shift;
        Jumped(oldTime, Now());
    }

    infra::Duration GetCurrentShift() const
    {
        return shift;
    }

private:
    TimerService& baseTimerService;
    infra::TimerSingleShot timer;
    infra::TimePoint nextTrigger;

    infra::Duration shift = infra::Duration();
};

class BeatTimerStub
    : public application::MetronomeBeatTimer
{
public:
    virtual void Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind) override;
    virtual void Stop() override;

private:
    infra::TimerRepeating timer;
};

void BeatTimerStub::Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind)
{
    timer.Start(std::chrono::microseconds(60000000 / bpm), [this]() { GetObserver().Beat(); }, infra::triggerImmediately);
}

void BeatTimerStub::Stop()
{
    timer.Cancel();
}

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hal::TimerServiceGeneric timerService;
    services::LowPowerStrategySdl lowPowerStrategy(timerService);
    infra::LowPowerEventDispatcher::WithSize<50> eventDispatcher(lowPowerStrategy);
    MyTimerService localTime(1, timerService);

    hal::DirectDisplaySdl display(infra::Vector(480, 272));

    BeatTimerStub beatTimer;
    DoubleBufferDisplayAdaptedFromDirectDisplay::WithStorage<480, 272, infra::PixelFormat::rgb565> displayAdapter(display);
    hal::BitmapPainterCanonical bitmapPainter;
    main_::Metronome metronome(display.Size(), localTime, beatTimer, displayAdapter, bitmapPainter);
    services::SdlTouchInteractor touchInteractor(lowPowerStrategy, metronome.touch);

    eventDispatcher.Run();

    return 0;
}
