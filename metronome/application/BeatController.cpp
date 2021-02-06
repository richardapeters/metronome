#include "metronome/application/BeatController.hpp"

namespace application
{
    BeatControllerImpl::BeatControllerImpl(BeatTimer& beatTimer)
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
        BeatTimerObserver::Subject().Start(bpm);
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
