#ifndef METRONOME_LOW_POWER_TIMER_HPP
#define METRONOME_LOW_POWER_TIMER_HPP

#include "cortex/InterruptCortex.hpp"
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
        infra::TimePoint Now() const;

    protected:
        virtual void Reload();

    private:
        infra::TimePoint now = infra::TimePoint();
        hal::ImmediateInterruptHandler timerInterrupt{ LPTIM1_IRQn, [this]() { Reload(); } };
        uint32_t reload = 0;
        uint32_t previousReload = 0;
    };

    class LowPowerTimerAlternatingReload
        : public LowPowerTimer
    {
    public:
        LowPowerTimerAlternatingReload();

    protected:
        virtual void Reload() override;

    private:
        int i = 0;
        std::array<int, 2> reload{ { 16384, 32768 } };
    };
}

#endif
