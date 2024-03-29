#include "hal/generic/TimerServiceGeneric.hpp"
#include "infra/event/LowPowerEventDispatcher.hpp"
#include "metronome/instantiations/Metronome.hpp"
#include "metronome/main_win/MultiBufferDisplayAdaptedFromDirectDisplay.hpp"
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
            timer.Start(nextTrigger, [this]()
                { Progressed(nextTrigger + shift); });
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
    : public application::BeatTimer
{
public:
    virtual void Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind) override;
    virtual void Stop() override;
    virtual void Gap(uint8_t gap) override;

private:
    infra::TimerRepeating timer;
    uint8_t subDivision = 0;

    uint8_t gap = 0;
    uint8_t gapIndex = 0;
};

void BeatTimerStub::Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind)
{
    subDivision = 0;
    NotifyObservers([bpm, beatsPerMeasure](auto& observer)
        { observer.Started(bpm, beatsPerMeasure); });
    timer.Start(
        std::chrono::microseconds(60000000 / bpm / 12), [this, beatsPerMeasure]()
        {
            NotifyObservers([this](auto& observer) { observer.Beat(subDivision % 12, gapIndex != 0); });
            ++subDivision;
            if (subDivision == beatsPerMeasure.ValueOr(1) * 12)
            {
                subDivision = 0;
                ++gapIndex;
                if (gapIndex > gap)
                    gapIndex = 0;
            }
        },
        infra::triggerImmediately);
}

void BeatTimerStub::Stop()
{
    NotifyObservers([](auto& observer)
        { observer.Stopped(); });
    timer.Cancel();
}

void BeatTimerStub::Gap(uint8_t gap)
{
    this->gap = gap;
}

class SerialCommunication8Beat
    : public hal::SerialCommunication
{
public:
    virtual void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion) override
    {}

    virtual void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) override
    {
        this->dataReceived = dataReceived;
    }

private:
    void Beat()
    {
        dataReceived(infra::MakeRange(*currentBeat));
        ++currentBeat;
        if (currentBeat == groove.end())
            currentBeat = groove.begin();
    }

private:
    std::array<std::array<uint8_t, 3>, 8> groove{ { { { 0x99, 36, 0x10 } },
        { { 0x99, 46, 0x10 } },
        { { 0x99, 38, 0x10 } },
        { { 0x99, 46, 0x10 } },
        { { 0x99, 36, 0x10 } },
        { { 0x99, 36, 0x10 } },
        { { 0x99, 38, 0x10 } },
        { { 0x99, 46, 0x10 } } } };

private:
    infra::Function<void(infra::ConstByteRange data)> dataReceived;
    infra::TimerRepeating timer{ infra::Duration(std::chrono::minutes(1)) / 120 / 2, [this]()
        {
            Beat();
        } };
    decltype(groove)::const_iterator currentBeat{ groove.begin() };
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hal::TimerServiceGeneric timerService;
    services::LowPowerStrategySdl lowPowerStrategy(timerService);
    infra::LowPowerEventDispatcher::WithSize<50> eventDispatcher(lowPowerStrategy);
    MyTimerService localTime(1, timerService);

    hal::DirectDisplaySdl display(infra::Vector(480, 272));

    BeatTimerStub beatTimer;
    services::MultiBufferDisplayAdaptedFromDirectDisplay displayAdapter(display);
    hal::BitmapPainterCanonical bitmapPainter;
    SerialCommunication8Beat serialMidi;
    infra::Bitmap::WithStorage<480, 272, infra::PixelFormat::rgb565> bitmap0;
    infra::Bitmap::WithStorage<480, 272, infra::PixelFormat::rgb565> bitmap1;
    infra::Bitmap::WithStorage<480, 272, infra::PixelFormat::rgb565> beatBitmap;
    main_::Metronome metronome(display.Size(), localTime, beatTimer, displayAdapter, bitmapPainter, serialMidi, bitmap0, bitmap1, beatBitmap);
    services::SdlTouchInteractor touchInteractor(lowPowerStrategy, metronome.touch);

    eventDispatcher.Run();

    return 0;
}
