#ifndef METRONOME_RTC_HPP
#define METRONOME_RTC_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"
#include "metronome/application/Ds3231.hpp"
#include "metronome/application/StuckI2cReleaser.hpp"

namespace main_
{
    struct Rtc
    {
        hal::GpioPinStm scl{ hal::Port::B, 8 };
        hal::GpioPinStm sda{ hal::Port::B, 9 };
        infra::Optional<hal::I2cStm> i2c;
        services::StuckI2cReleaser stuckI2cReleaser{ scl, sda, [this]()->hal::I2cMaster& { i2c.Emplace(1, scl, sda); return *i2c; } };

        hal::GpioPinStm rtcInterrupt{ hal::Port::G, 3 };
        services::Ds3231TimerService rtc{ stuckI2cReleaser, rtcInterrupt, 1 };
    };
}

#endif
