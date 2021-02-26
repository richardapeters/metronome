#include "infra/event/EventDispatcher.hpp"
#include "metronome/application/BeatController.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace application
{
    BeatControllerImpl::BeatControllerImpl(MetronomeBeatTimer& beatTimer)
        : BeatTimerObserver(beatTimer)
    {}

    void BeatControllerImpl::SetBpm(uint16_t newBpm)
    {
        bpm = newBpm;
        if (Running())
            Start();
    }

    void BeatControllerImpl::Start()
    {
        if (Running())
            Stop();

        running = true;
        PrepareNextBeat();
        MetronomePainterSubject::GetObserver().StopAutomaticPainting();
        BeatController::GetObserver().BeatOn();
        MetronomePainterSubject::GetObserver().ManualPaint();
        swap = true;
        BeatTimerObserver::Subject().Start(bpm, beatsPerMeasure, noteKind);
    }

    void BeatControllerImpl::Stop()
    {
        running = false;
        BeatTimerObserver::Subject().Stop();
        prepareBeat.Cancel();
    }

    bool BeatControllerImpl::Running() const
    {
        return running;
    }

    void BeatControllerImpl::SelectedBeatsPerMeasure(uint8_t beatsPerMeasure)
    {
        this->beatsPerMeasure = beatsPerMeasure;

        if (Running())
            Start();
    }

    void BeatControllerImpl::DisabledBeatsPerMinute()
    {
        beatsPerMeasure = infra::none;

        if (Running())
            Start();
    }

    void BeatControllerImpl::SelectedNoteKind(uint8_t noteKind)
    {
        this->noteKind = noteKind;

        if (Running())
            Start();
    }

    void BeatControllerImpl::Beat()
    {
        if (swap.exchange(false))
            MetronomePainterSubject::GetObserver().SwapLayers([this]() { MetronomePainterSubject::GetObserver().StartAutomaticPainting(); });
        else
            infra::EventDispatcher::Instance().Schedule([this]()
            {
                expectedPaintDuration += std::chrono::milliseconds(5);
                services::GlobalTracer().Trace() << "Increasing duration to " << std::chrono::duration_cast<std::chrono::milliseconds>(expectedPaintDuration).count() << "ms";
                MetronomePainterSubject::GetObserver().SwapLayers([this]() { MetronomePainterSubject::GetObserver().StartAutomaticPainting(); });
            });

        infra::EventDispatcher::Instance().Schedule([this]()
        {
            PrepareNextBeat();

            beatOff.Start(std::chrono::milliseconds(30), [this]() { BeatController::GetObserver().BeatOff(); });
        });
    }

    void BeatControllerImpl::PrepareNextBeat()
    {
        holdPaint.Start(std::chrono::microseconds(60000000 / bpm) - 2 * expectedPaintDuration, [this]()
        {
            MetronomePainterSubject::GetObserver().StopAutomaticPainting();
        });

        prepareBeat.Start(std::chrono::microseconds(60000000 / bpm) - expectedPaintDuration, [this]()
        {
            BeatController::GetObserver().BeatOn();
            auto start = infra::Now();
            MetronomePainterSubject::GetObserver().ManualPaint();
            auto duration = infra::Now() - start;
            swap = true;
            services::GlobalTracer().Trace() << "Paint duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        });
    }
}
