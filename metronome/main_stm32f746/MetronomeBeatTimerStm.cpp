#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"

namespace application
{
    MetronomeBeatTimerStm::MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> data)
        : sai(sai)
        , data(data)
    {}

    void MetronomeBeatTimerStm::Start(uint16_t bpm)
    {
        SetReload(std::min(32768 * 60 / bpm, 65535));
        hal::LowPowerTimer::Start();
    }

    void MetronomeBeatTimerStm::Stop()
    {
        hal::LowPowerTimer::Stop();
    }

    void MetronomeBeatTimerStm::Reload()
    {
        sai.Transfer(data);
        hal::LowPowerTimer::Reload();
        GetObserver().Beat();
    }
}
