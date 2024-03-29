#ifndef METRONOME_METRONOME_BEAT_TIMER_STM_HPP
#define METRONOME_METRONOME_BEAT_TIMER_STM_HPP

#include "local_hal/stm32fxxx/SaiStm.hpp"
#include "metronome/interfaces/BeatTimer.hpp"
#include "metronome/main_stm32f746/LowPowerTimer.hpp"

namespace application
{
    class MetronomeBeatTimerStm
        : public BeatTimer
        , private hal::LowPowerTimer
    {
    public:
        MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const int16_t> dataAccent, infra::MemoryRange<const int16_t> data, infra::MemoryRange<const int16_t> dataSub);

        virtual void Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind) override;
        virtual void Stop() override;
        virtual void Gap(uint8_t gap) override;

    protected:
        virtual void Reload() override;

    private:
        void TransferBeat();
        void SetNextReload();

    private:
        hal::SaiStm& sai;
        infra::MemoryRange<const int16_t> dataAccent;
        infra::MemoryRange<const int16_t> data;
        infra::MemoryRange<const int16_t> dataSub;

        uint16_t bpm;
        infra::Optional<uint8_t> beatsPerMeasure;
        uint8_t noteKind;

        uint32_t step;
        uint32_t amount = 0;
        uint32_t total;
        uint32_t cumulative = 0;
        uint8_t currentBeat;

        uint8_t gap = 0;
        uint8_t gapIndex = 0;
    };
}

#endif
