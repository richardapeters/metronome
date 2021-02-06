#include "metronome/application/BeatController.hpp"

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

    void BeatControllerImpl::Start()
    {
        if (Running())
            Stop();

        beat.Start(std::chrono::microseconds(60000000 / bpm), [this]() { Beat(); });
        beatTimer.Start(bpm);
    }

    void BeatControllerImpl::Stop()
    {
        beatTimer.Stop();
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
