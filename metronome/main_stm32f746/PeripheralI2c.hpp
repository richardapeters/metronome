#ifndef METRONOME_PERIPHERAL_I2C_HPP
#define METRONOME_PERIPHERAL_I2C_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"
#include "metronome/application/StuckI2cReleaser.hpp"
#include "services/util/I2cMultipleAccess.hpp"

namespace main_
{
    struct PeripheralI2c
    {
        hal::GpioPinStm scl{ hal::Port::H, 7 };
        hal::GpioPinStm sda{ hal::Port::H, 8 };
        infra::Optional<hal::I2cStm> i2c;
        services::StuckI2cReleaser stuckI2cReleaser{ scl, sda, [this]()->hal::I2cMaster& { i2c.Emplace(3, scl, sda); return *i2c; } };
        services::I2cMultipleAccessMaster i2cMaster{ stuckI2cReleaser };
        services::I2cMultipleAccess i2cTouch{ i2cMaster };
        services::I2cMultipleAccess i2cAudio{ i2cMaster };
    };
}

#endif
