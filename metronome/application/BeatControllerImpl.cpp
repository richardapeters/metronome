#include "infra/event/EventDispatcher.hpp"
#include "metronome/application/BeatControllerImpl.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace application
{
    BeatControllerImpl::BeatControllerImpl(BeatTimer& beatTimer)
        : beatTimer(beatTimer)
    {}

    void BeatControllerImpl::SetBpm(uint16_t newBpm)
    {
        bpm = newBpm;
        if (Running())
            Start();
    }

    uint16_t BeatControllerImpl::MakeBpmValid(uint16_t bpm) const
    {
        return std::min(300, std::max(40, bpm + 0));
    }

    bool BeatControllerImpl::BpmIsValid(uint16_t bpm) const
    {
        return MakeBpmValid(bpm) == bpm;
    }

    void BeatControllerImpl::Start()
    {
        Stop();

        runningRequested = true;
        EvaluateRunningRequested();
    }

    void BeatControllerImpl::Stop()
    {
        if (Running())
            runningState->Stop();
    }

    bool BeatControllerImpl::Running() const
    {
        return runningState != infra::none;
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

    void BeatControllerImpl::EvaluateRunningRequested()
    {
        if (runningRequested && !runningState)
        {
            runningRequested = false;
            runningState.Emplace(*this);
        }
    }

    void BeatControllerImpl::RunningStopped()
    {
        runningState = infra::none;
        EvaluateRunningRequested();
    }

    BeatControllerImpl::RunningState::RunningState(BeatControllerImpl& controller)
        : controller(controller)
    {
        controller.beatTimer.Start(controller.bpm, controller.beatsPerMeasure, controller.noteKind);
    }

    void BeatControllerImpl::RunningState::Stop()
    {
        if (stopRequested)
            return;

        controller.beatTimer.Stop();
        controller.BeatController::GetObserver().BeatOff();

        stopRequested = true;

        controller.RunningStopped();
    }
}
