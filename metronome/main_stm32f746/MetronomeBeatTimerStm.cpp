#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace application
{
    MetronomeBeatTimerStm::MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> data)
        : sai(sai)
        , data(data)
    {}

    void MetronomeBeatTimerStm::Start(uint16_t bpm)
    {
        this->bpm = std::max<uint16_t>(bpm, 31); // Ensure step <= 65536

        hal::LowPowerTimer::Start();
        Reload();
    }

    void MetronomeBeatTimerStm::Stop()
    {
        hal::LowPowerTimer::Stop();
        cumulative = 0;
        amount = 0;
    }

    void MetronomeBeatTimerStm::Reload()
    {
        sai.Transfer(data);
        SetNextReload();
        hal::LowPowerTimer::Reload();
        GetObserver().Beat();
    }

    void MetronomeBeatTimerStm::SetNextReload()
    {
        if (amount == 0)
        {
            total = 32768 * 60;
            step = std::min<uint32_t>(total / bpm, 65535);
            amount = total / step;
            cumulative = 0;
        }

        --amount;

        if (cumulative + step + amount * step < total)
            ++step;
        else if (cumulative + step + amount * step > total)
            --step;

        services::GlobalTracer().Trace() << "Step: " << step << " amount: " << amount;

        cumulative += step;

        SetReload(step);
    }
}
