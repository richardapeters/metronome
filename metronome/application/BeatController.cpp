#include "metronome/application/BeatController.hpp"

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
        BeatTimerObserver::Subject().Start(bpm, beatsPerMeasure);
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

    void BeatControllerImpl::Beat()
    {
        MetronomePainterSubject::GetObserver().SwapLayers([this]() { MetronomePainterSubject::GetObserver().StartAutomaticPainting(); });
        PrepareNextBeat();

        beatOff.Start(std::chrono::milliseconds(30), [this]() { BeatController::GetObserver().BeatOff(); });
    }

    void BeatControllerImpl::PrepareNextBeat()
    {
        holdPaint.Start(std::chrono::microseconds(60000000 / bpm) - std::chrono::milliseconds(160), [this]()
        {
            MetronomePainterSubject::GetObserver().StopAutomaticPainting();
        });

        prepareBeat.Start(std::chrono::microseconds(60000000 / bpm) - std::chrono::milliseconds(80), [this]()
        {
            BeatController::GetObserver().BeatOn();
            auto start = infra::Now();
            MetronomePainterSubject::GetObserver().ManualPaint();
            auto duration = infra::Now() - start;
            //services::GlobalTracer().Trace() << "Paint duration: " << std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        });
    }
}
