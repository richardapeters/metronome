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

    void BeatControllerImpl::Beat()
    {
        if (Running())
            runningState->Beat();
    }

    void BeatControllerImpl::EvaluateRunningRequested()
    {
        if (runningRequested && !runningState)
        {
            runningRequested = false;
            runningState.Emplace(*this);
            runningState->Beat();
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
        beat = holdPaint.Now();
        PrepareNextBeat();
        controller.MetronomePainterSubject::GetObserver().StopAutomaticPainting();
        BeatOn();
        controller.BeatTimerObserver::Subject().Start(controller.bpm, controller.beatsPerMeasure, controller.noteKind);
    }

    void BeatControllerImpl::RunningState::Stop()
    {
        if (stopRequested)
            return;

        holdPaint.Cancel();
        prepareBeat.Cancel();
        beatOff.Cancel();
        controller.BeatTimerObserver::Subject().Stop();
        controller.BeatController::GetObserver().BeatOff();

        stopRequested = true;

        switch (state)
        {
            case State::idle:
                controller.RunningStopped();
                break;
            case State::stoppedAutomaticPainting:
                controller.MetronomePainterSubject::GetObserver().StartAutomaticPainting();
                controller.RunningStopped();
                break;
            case State::manuallyPainting:
            case State::readyForSwap:
            case State::tooLateForSwapping:
                controller.MetronomePainterSubject::GetObserver().SwapLayers([this]()
                {
                    controller.BeatController::GetObserver().BeatOff();
                    controller.MetronomePainterSubject::GetObserver().StartAutomaticPainting();
                    controller.RunningStopped();
                });
                break;
            default:
                std::abort();
        }
    }

    void BeatControllerImpl::RunningState::Beat()
    {
        beat = holdPaint.Now();
        State expected = State::readyForSwap;
        if (state.compare_exchange_strong(expected, State::readyForSwap))
            SwapLayers();
        else
        {
            State expected = State::stoppedAutomaticPainting;
            state.compare_exchange_strong(expected, State::tooLateForSwapping);
        }
    }

    void BeatControllerImpl::RunningState::PrepareNextBeat()
    {
        holdPaint.Start(beat + std::chrono::microseconds(60000000 / controller.bpm) - 2 * controller.expectedPaintDuration, [this]()
        {
            State expected = State::idle;
            if (state.compare_exchange_strong(expected, State::stoppedAutomaticPainting))
                controller.MetronomePainterSubject::GetObserver().StopAutomaticPainting();
        });

        prepareBeat.Start(beat + std::chrono::microseconds(60000000 / controller.bpm) - controller.expectedPaintDuration, [this]()
        {
            State expected = State::stoppedAutomaticPainting;
            if (state.compare_exchange_strong(expected, State::manuallyPainting))
                BeatOn();
            else
            {
                State expected = State::tooLateForSwapping;
                if (state.compare_exchange_strong(expected, State::tooLateForSwapping))
                    BeatOn();
            }
        });
    }

    void BeatControllerImpl::RunningState::BeatOn()
    {
        controller.BeatController::GetObserver().BeatOn();
        controller.MetronomePainterSubject::GetObserver().ManualPaint();

        State expected = State::manuallyPainting;
        if (!state.compare_exchange_strong(expected, State::readyForSwap))
        {
            State expected = State::tooLateForSwapping;
            if (state.compare_exchange_strong(expected, State::idle))
                SwapLayers();
        }
    }

    void BeatControllerImpl::RunningState::SwapLayers()
    {
        controller.MetronomePainterSubject::GetObserver().SwapLayers([this]()
        {
            state = State::idle;
            controller.MetronomePainterSubject::GetObserver().StartAutomaticPainting();

            beatOff.Start(beat + std::chrono::milliseconds(30), [this]()
                {
                    controller.BeatController::GetObserver().BeatOff();
                    PrepareNextBeat();
                });
            if (stopRequested)
                controller.RunningStopped();
        });
    }
}
