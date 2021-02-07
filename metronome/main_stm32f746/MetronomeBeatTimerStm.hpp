#ifndef METRONOME_METRONOME_BEAT_TIMER_STM_HPP
#define METRONOME_METRONOME_BEAT_TIMER_STM_HPP

#include "local_hal/stm32fxxx/SaiStm.hpp"
#include "metronome/application/BeatController.hpp"
#include "metronome/main_stm32f746/LowPowerTimer.hpp"

namespace application
{
    class MetronomeBeatTimerStm
        : public MetronomeBeatTimer
        , private hal::LowPowerTimer
    {
    public:
        MetronomeBeatTimerStm(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> data);

        virtual void Start(uint16_t bpm) override;
        virtual void Stop() override;

    protected:
        virtual void Reload() override;

    private:
        void SetNextReload();

    private:
        hal::SaiStm& sai;
        infra::MemoryRange<const uint16_t> data;

        uint16_t bpm;
        uint32_t step;
        uint32_t amount = 0;
        uint32_t total;
        uint32_t cumulative = 0;
    };
}

#endif
