#ifndef METRONOME_TOUCH_HPP
#define METRONOME_TOUCH_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "metronome/application/TouchFt5x06.hpp"

namespace main_
{
    struct Touch
    {
        Touch(hal::I2cMaster& i2c, services::TouchRecipient& touchRecipient);

        hal::GpioPinStm lcdInt;
        application::TouchFt5x06OnTouchRecipient touch;
    };
}

#endif
