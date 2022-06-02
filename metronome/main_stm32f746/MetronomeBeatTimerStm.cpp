#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"

namespace application
{
    namespace
    {
        static const std::array<uint8_t, 5> subSteps{ 1, 1, 1, 2, 3 };
        static const std::array<uint8_t, 5> superSteps{ 100, 2, 1, 1, 1 };
    }

    MetronomeBeatTimerStm::MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const int16_t> dataAccent, infra::MemoryRange<const int16_t> data, infra::MemoryRange<const int16_t> dataSub)
        : sai(sai)
        , dataAccent(dataAccent)
        , data(data)
        , dataSub(dataSub)
    {}

    void MetronomeBeatTimerStm::Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind)
    {
        this->bpm = std::max<uint16_t>(bpm, 31); // Ensure step <= 65536
        this->beatsPerMeasure = beatsPerMeasure;
        this->noteKind = noteKind;

        currentBeat = 0;

        NotifyObservers([bpm, beatsPerMeasure](auto& observer) { observer.Started(bpm, beatsPerMeasure); });
        hal::LowPowerTimer::Start();
        Reload();
    }

    void MetronomeBeatTimerStm::Stop()
    {
        NotifyObservers([](auto& observer) { observer.Stopped(); });
        hal::LowPowerTimer::Stop();
        cumulative = 0;
        amount = 0;
    }

    void MetronomeBeatTimerStm::Reload()
    {
        if (beatsPerMeasure != infra::none && currentBeat == 0)
            sai.Transfer(dataAccent);
        else if (currentBeat % subSteps[noteKind] == 0)
        {
            if (currentBeat % superSteps[noteKind] == 0)
                sai.Transfer(data);
        }
        else
            sai.Transfer(dataSub);

        SetNextReload();
        hal::LowPowerTimer::Reload();

        if (currentBeat % subSteps[noteKind] == 0)
            NotifyObservers([](auto& observer) { observer.Beat(); });

        ++currentBeat;
        if (currentBeat == beatsPerMeasure.ValueOr(1) * subSteps[noteKind])
            currentBeat = 0;
    }

    void MetronomeBeatTimerStm::SetNextReload()
    {
        if (amount == 0)
        {
            total = 32768 * 60;
            step = std::min<uint32_t>(total / bpm / subSteps[noteKind], 65535);
            amount = total / step;
            cumulative = 0;
        }

        --amount;

        if (cumulative + step + amount * step < total)
            ++step;
        else if (cumulative + step + amount * step > total)
            --step;

        cumulative += step;

        SetReload(step);
    }
}
