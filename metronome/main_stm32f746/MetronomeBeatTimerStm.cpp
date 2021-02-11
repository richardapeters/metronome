#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace application
{
    MetronomeBeatTimerStm::MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> dataAccent, infra::MemoryRange<const uint16_t> data)
        : sai(sai)
        , dataAccent(dataAccent)
        , data(data)
    {}

    void MetronomeBeatTimerStm::Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure)
    {
        this->bpm = std::max<uint16_t>(bpm, 31); // Ensure step <= 65536
        this->beatsPerMeasure = beatsPerMeasure;

        currentBeat = 0;

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
    	if (beatsPerMeasure != infra::none && currentBeat == 0)
    		sai.Transfer(dataAccent);
    	else
    		sai.Transfer(data);

        SetNextReload();
        hal::LowPowerTimer::Reload();
        GetObserver().Beat();

        if (beatsPerMeasure != infra::none)
        {
        	++currentBeat;
        	if (currentBeat == beatsPerMeasure)
        		currentBeat = 0;
        }
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
