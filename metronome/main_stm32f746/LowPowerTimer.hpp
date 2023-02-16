#ifndef METRONOME_LOW_POWER_TIMER_HPP
#define METRONOME_LOW_POWER_TIMER_HPP

#include "hal_st/cortex/InterruptCortex.hpp"
#include "infra/timer/Timer.hpp"

namespace hal
{
    class LowPowerTimer
    {
    public:
        LowPowerTimer();
        ~LowPowerTimer();

        void SetReload(uint32_t reload);
        void Start();
        void Stop();
        infra::TimePoint Now() const;

    protected:
        virtual void Reload();

    private:
        infra::TimePoint now = infra::TimePoint();
        hal::ImmediateInterruptHandler timerInterrupt{ LPTIM1_IRQn, [this]()
            {
                Reload();
            } };
        uint32_t reload = 32768;
        uint32_t previousReload = reload;
    };
}

#endif
