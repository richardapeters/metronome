#include "hal/generic/TimerServiceGeneric.hpp"
#include "infra/event/LowPowerEventDispatcher.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/ViewDateEntry.hpp"
#include "metronome/main_win/DoubleBufferDisplayAdaptedFromDirectDisplay.hpp"
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hal::TimerServiceGeneric timerService;
    services::LowPowerStrategySdl lowPowerStrategy(timerService);
    infra::LowPowerEventDispatcher::WithSize<50> eventDispatcher(lowPowerStrategy);
    MyTimerService localTime(1, timerService);

    hal::DirectDisplaySdl display(infra::Vector(480, 272));

    BeatTimerStub beatTimer;
    services::DoubleBufferDisplayAdaptedFromDirectDisplay::WithStorage<480, 272, infra::PixelFormat::rgb565> displayAdapter(display);
    hal::BitmapPainterCanonical bitmapPainter;
    main_::Metronome metronome(display.Size(), localTime, beatTimer, displayAdapter, bitmapPainter);
    services::SdlTouchInteractor touchInteractor(lowPowerStrategy, metronome.touch);

    eventDispatcher.Run();

    return 0;
}
