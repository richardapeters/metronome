#ifndef METRONOME_DS3231_HPP
#define METRONOME_DS3231_HPP

#include "hal/interfaces/Gpio.hpp"
#include "hal/interfaces/I2c.hpp"
#include "infra/timer/TimerService.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "infra/util/Optional.hpp"

namespace services
{
    class Ds3231
    {
    public:
        Ds3231(hal::I2cMaster& i2c, const infra::Function<void(infra::Optional<infra::TimePoint> time)>& onInitialized);

        void ReadTime(const infra::Function<void(infra::TimePoint time)>& onDone);
        void SetTime(infra::TimePoint time, const infra::Function<void(infra::TimePoint)>& onDone);

    private:
        struct TimeData
        {
            uint8_t seconds;
            uint8_t minutes;
            uint8_t hours;
            uint8_t day;
            uint8_t date;
            uint8_t month;
            uint8_t year;
        };

    private:
        void Initialize();
        infra::TimePoint Convert(TimeData data) const;
        uint8_t FromBcd(uint8_t bcd) const;
        TimeData Convert(infra::TimePoint time) const;
        uint8_t ToBcd(uint8_t x) const;

    private:
        hal::I2cMaster& i2c;
        infra::AutoResetFunction<void(infra::Optional<infra::TimePoint> time)> onInitialized;
        infra::AutoResetFunction<void(infra::TimePoint time)> onReadDone;
        infra::AutoResetFunction<void(infra::TimePoint time)> onSetDone;

        uint8_t status;
        TimeData timeData;
    };

    class SettableTimerService
        : public infra::TimerService
    {
    public:
        using infra::TimerService::TimerService;

        virtual void SetTime(infra::TimePoint time, const infra::Function<void()>& onDone) = 0;
    };

    class Ds3231TimerService
        : public SettableTimerService
    {
    public:
        Ds3231TimerService(hal::I2cMaster& i2c, hal::GpioPin& interrupt, uint32_t id);

        virtual void SetTime(infra::TimePoint time, const infra::Function<void()>& onDone) override;

        virtual infra::TimePoint Now() const override;
        virtual infra::Duration Resolution() const override;

    private:
        void OnInitialized(infra::Optional<infra::TimePoint> newTime);
        void EnableInterrupt();
        void SecondElapsed();

    private:
        Ds3231 rtc;
        bool interruptEnabled = false;
        hal::InputPin interrupt;
        infra::Optional<infra::TimePoint> time;
        infra::AutoResetFunction<void()> onSetDone;
    };
}

#endif
