#ifndef METRONOME_BEAT_CONTROLLER_HPP
#define METRONOME_BEAT_CONTROLLER_HPP

#include "infra/util/Observer.hpp"
#include "infra/util/Optional.hpp"
#include "infra/timer/Timer.hpp"
#include <atomic>

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

    class MetronomeBeatTimer;

    class BeatTimerObserver
        : public infra::Observer<BeatTimerObserver, MetronomeBeatTimer>
    {
    public:
        using infra::Observer<BeatTimerObserver, MetronomeBeatTimer>::Observer;

        virtual void Beat() = 0;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure) = 0;
        virtual void Stopped() = 0;
    };

    class MetronomeBeatTimer
        : public infra::Subject<BeatTimerObserver>
    {
    public:
        virtual void Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind) = 0;
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
        virtual uint16_t MakeBpmValid(uint16_t bpm) const = 0;
        virtual bool BpmIsValid(uint16_t bpm) const = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool Running() const = 0;

        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) = 0;
        virtual void DisabledBeatsPerMinute() = 0;
        virtual void SelectedNoteKind(uint8_t noteKind) = 0;
    };

    class BeatControllerImpl
        : public BeatController
        , public BeatTimerObserver
        , public MetronomePainterSubject
    {
    public:
        BeatControllerImpl(MetronomeBeatTimer& beatTimer);

        virtual void SetBpm(uint16_t newBpm) override;
        virtual uint16_t MakeBpmValid(uint16_t bpm) const override;
        virtual bool BpmIsValid(uint16_t bpm) const override;
        virtual void Start() override;
        virtual void Stop() override;
        virtual bool Running() const override;
        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) override;
        virtual void DisabledBeatsPerMinute() override;
        virtual void SelectedNoteKind(uint8_t noteKind) override;

        // Implementation of BeatTimerObserver
        virtual void Beat() override;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure) override;
        virtual void Stopped() override;

    private:
        void EvaluateRunningRequested();
        void RunningStopped();

    private:
        class RunningState
        {
        public:
            RunningState(BeatControllerImpl& controller);

            void Stop();
            void Beat();

        private:
            void PrepareNextBeat();
            void BeatOn();
            void SwapLayers();

        private:
            BeatControllerImpl& controller;

            infra::TimerSingleShot holdPaint;
            infra::TimerSingleShot prepareBeat;
            infra::TimerSingleShot beatOff;

            enum class State
            {
                idle,
                stoppedAutomaticPainting,
                manuallyPainting,
                readyForSwap,
                swapping,
                tooLateForSwapping
            };

            std::atomic<State> state{ State::manuallyPainting };
            bool stopRequested = false;
            infra::TimePoint beat;
        };

    private:
        uint16_t bpm;
        infra::Optional<uint8_t> beatsPerMeasure;
        uint8_t noteKind = 0;

        infra::Duration expectedPaintDuration = std::chrono::milliseconds(120);

        infra::Optional<RunningState> runningState;
        bool runningRequested = false;
    };
}

#endif
