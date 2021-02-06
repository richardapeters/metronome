#ifndef METRONOME_BEAT_CONTROLLER_HPP
#define METRONOME_BEAT_CONTROLLER_HPP

#include "infra/util/Observer.hpp"
#include "infra/timer/Timer.hpp"

namespace application
{
    class MetronomePainterSubject;

    class MetronomePainter
        : public infra::SingleObserver<MetronomePainter, MetronomePainterSubject>
    {
    public:
        using infra::SingleObserver<MetronomePainter, MetronomePainterSubject>::SingleObserver;

        virtual void StopAutomaticPainting() = 0;
        virtual void StartAutomaticPainting() = 0;
        virtual void ManualPaint() = 0;
        virtual void SwapLayers(infra::Function<void()> onDone) = 0;
    };

    class MetronomePainterSubject
        : public infra::Subject<MetronomePainter>
    {};

    class BeatTimer;

    class BeatTimerObserver
        : public infra::SingleObserver<BeatTimerObserver, BeatTimer>
    {
    public:
        using infra::SingleObserver<BeatTimerObserver, BeatTimer>::SingleObserver;

        virtual void Beat() = 0;
    };

    class BeatTimer
        : public infra::Subject<BeatTimerObserver>
    {
    public:
        virtual void Start(uint16_t bpm) = 0;
        virtual void Stop() = 0;
    };

    class BeatController;

    class BeatControllerObserver
        : public infra::SingleObserver<BeatControllerObserver, BeatController>
    {
    public:
        using infra::SingleObserver<BeatControllerObserver, BeatController>::SingleObserver;

        virtual void BeatOn() = 0;
        virtual void BeatOff() = 0;
    };

    class BeatController
        : public infra::Subject<BeatControllerObserver>
    {
    public:
        virtual void SetBpm(uint16_t newBpm) = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool Running() const = 0;
    };

    class BeatControllerImpl
        : public BeatController
        , public BeatTimerObserver
        , public MetronomePainterSubject
    {
    public:
        BeatControllerImpl(BeatTimer& beatTimer);

        virtual void SetBpm(uint16_t newBpm) override;
        virtual void Start() override;
        virtual void Stop() override;
        virtual bool Running() const override;

        virtual void Beat() override;

    private:
        void PrepareNextBeat();

    private:
        infra::TimerSingleShot holdPaint;
        infra::TimerSingleShot prepareBeat;
        infra::TimerSingleShot beatOff;
        uint16_t bpm;
        bool running = false;
    };
}

#endif
