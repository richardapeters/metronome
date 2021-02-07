#include "hal/generic/TimerServiceGeneric.hpp"
#include "infra/event/LowPowerEventDispatcher.hpp"
#include "infra/timer/DerivedTimerService.hpp"
#include "infra/util/PostAssign.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/ViewDateEntry.hpp"
#include "preview/interfaces/ViewPainterDirectDisplay.hpp"
#include "preview/sdl/DirectDisplaySdl.hpp"
#include "preview/sdl/LowPowerStrategySdl.hpp"
#include "preview/sdl/SdlTouchInteractor.hpp"
#include <Windows.h>

namespace application
{
    class MetronomeRefresher
        : public services::View
    {
    public:
        MetronomeRefresher(services::ViewPainter& viewPainter, services::View& view);

        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;

    protected:
        virtual void Dirty(infra::Region region) override;

    private:
        void Paint();
        void PaintDone();

    private:
        services::ViewPainter& viewPainter;
        services::View& view;
        infra::Region dirtyRegion;

        bool scheduled = false;
    };

    MetronomeRefresher::MetronomeRefresher(services::ViewPainter& viewPainter, services::View& view)
        : viewPainter(viewPainter)
        , view(view)
    {
        view.SetParent(*this);
        Dirty(view.ViewRegion());
    }

    void MetronomeRefresher::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        std::abort();
    }

    void MetronomeRefresher::Dirty(infra::Region region)
    {
        dirtyRegion = dirtyRegion | region;

        if (!scheduled)
        {
            scheduled = true;
            infra::EventDispatcher::Instance().Schedule([this]() { Paint(); });
        }
    }

    void MetronomeRefresher::Paint()
    {
        viewPainter.Paint(view, infra::PostAssign(dirtyRegion, infra::Region()), [this]() { PaintDone(); });
    }

    void MetronomeRefresher::PaintDone()
    {
        scheduled = false;
        if (!dirtyRegion.Empty())
            Dirty(dirtyRegion);
    }
}

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
    virtual void Start(uint16_t bpm) override;
    virtual void Stop() override;

private:
    infra::TimerRepeating timer;
};

void BeatTimerStub::Start(uint16_t bpm)
{
    timer.Start(std::chrono::microseconds(60000000 / bpm), [this]() { GetObserver().Beat(); });
}

void BeatTimerStub::Stop()
{
    timer.Cancel();
}

class DisplayAdapter
    : public hal::DoubleBufferDisplay
{
public:
    DisplayAdapter(hal::DirectDisplay& display);

    virtual void SwapLayers(const infra::Function<void()>& onDone) override;
    virtual infra::Bitmap& DrawingBitmap() override;
    virtual const infra::Bitmap& ViewingBitmap() const override;

private:
    hal::DirectDisplay& display;
    infra::Bitmap::WithStorage<480, 272, infra::PixelFormat::rgb565> drawingBitmap;
    infra::Bitmap::WithStorage<480, 272, infra::PixelFormat::rgb565> viewingBitmap;
};

DisplayAdapter::DisplayAdapter(hal::DirectDisplay& display)
    : display(display)
{}

void DisplayAdapter::SwapLayers(const infra::Function<void()>& onDone)
{
    std::swap(drawingBitmap, viewingBitmap);
    display.DrawBitmap(infra::Point(), viewingBitmap, infra::Region(infra::Point(), display.Size()));
    onDone();
}

infra::Bitmap& DisplayAdapter::DrawingBitmap()
{
    return drawingBitmap;
}

const infra::Bitmap& DisplayAdapter::ViewingBitmap() const
{
    return viewingBitmap;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hal::TimerServiceGeneric timerService;
    services::LowPowerStrategySdl lowPowerStrategy(timerService);
    infra::LowPowerEventDispatcher::WithSize<50> eventDispatcher(lowPowerStrategy);
    MyTimerService localTime(1, timerService);

    hal::DirectDisplaySdl display(infra::Vector(480, 272));

    BeatTimerStub beatTimer;
    DisplayAdapter displayAdapter(display);
    hal::BitmapPainterCanonical bitmapPainter;
    main_::Metronome metronome(display.Size(), localTime, beatTimer, displayAdapter, bitmapPainter);
    services::SdlTouchInteractor touchInteractor(lowPowerStrategy, metronome.touch);

    services::ViewPainterDirectDisplay viewPainter(display);
    application::MetronomeRefresher refresher(viewPainter, metronome.touch.GetView());

    eventDispatcher.Run();

    return 0;
}
