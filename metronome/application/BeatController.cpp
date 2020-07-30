#include "metronome/application/BeatController.hpp"

namespace application
{
    void BeatControllerImpl::SetBpm(uint16_t newBpm)
    {
        bpm = newBpm;
        if (Running())
            Start();
    }

    void BeatControllerImpl::Start()
    {
        beat.Start(std::chrono::microseconds(60000000 / bpm), [this]() { Beat(); });
    }

    void BeatControllerImpl::Stop()
    {
        beat.Cancel();
    }

    bool BeatControllerImpl::Running() const
    {
        return beat.Armed();
    }

    void BeatControllerImpl::Beat()
    {
        NotifyObservers([](BeatControllerObserver& observer) { observer.PrepareBeat(); });
    }
}
