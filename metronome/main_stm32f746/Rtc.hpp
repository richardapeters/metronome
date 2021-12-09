#ifndef METRONOME_RTC_HPP
#define METRONOME_RTC_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"
#include "metronome/application/Ds3231.hpp"
#include "metronome/application/StuckI2cReleaser.hpp"

namespace services
{
    class SwitchableSettableTimerService
        : public SettableTimerService
        , private infra::Timer
    {
    public:
        SwitchableSettableTimerService(uint32_t id, SettableTimerService& delegated)
            : SettableTimerService(id)
            , infra::Timer(delegated.Id())
            , delegated(delegated)
        {}

        void Disable()
        {
            enabled = false;
        }

        virtual infra::TimePoint Now() const override
        {
            if (enabled)
                return delegated.Now();
            else
                return infra::TimePoint();
        }

        virtual infra::Duration Resolution() const override
        {
            if (enabled)
                return delegated.Resolution();
            else
                return infra::Duration();
        }

        virtual void SetTime(infra::TimePoint time, const infra::Function<void()>& onDone) override
        {
            if (enabled)
                delegated.SetTime(time, onDone);
            else
                onDone();
        }

        virtual void NextTriggerChanged() override
        {
            ComputeNextTriggerTime();
        }

    private:
        virtual void ComputeNextTriggerTime() override
        {
            SetNextTriggerTime(infra::TimerService::NextTrigger(), [this]() { Progressed(Now()); });
        }

        virtual void Jumped(infra::TimePoint from, infra::TimePoint to) override
        {
            infra::TimerService::Jumped(from, to);
        }

    private:
        SettableTimerService& delegated;
        bool enabled = true;
    };

    class I2cStmHandleLostDevices
        : public hal::I2cStm
    {
    public:
        I2cStmHandleLostDevices(uint8_t oneBasedI2cIndex, hal::GpioPinStm& scl, hal::GpioPinStm& sda, const infra::Function<void()>& onNotFound)
            : hal::I2cStm(oneBasedI2cIndex, scl, sda)
            , onNotFound(onNotFound)
        {}

    protected:
        virtual void DeviceNotFound() override { onNotFound(); }

    private:
        infra::Function<void()> onNotFound;
    };
}

namespace main_
{
    struct Rtc
    {
        hal::GpioPinStm scl{ hal::Port::B, 8 };
        hal::GpioPinStm sda{ hal::Port::B, 9 };
        infra::Optional<services::I2cStmHandleLostDevices> i2c;
        services::StuckI2cReleaser stuckI2cReleaser{ scl, sda, [this]()->hal::I2cMaster& { i2c.Emplace(1, scl, sda, [this]() { rtc.Disable(); }); return *i2c; } };

        hal::GpioPinStm rtcInterrupt{ hal::Port::G, 3 };
        services::Ds3231TimerService ds3231{ stuckI2cReleaser, rtcInterrupt, 1 };
        services::SwitchableSettableTimerService rtc{ 2, ds3231 };
    };
}

#endif
