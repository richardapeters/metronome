#include "metronome/main_stm32f746/Touch.hpp"

namespace main_
{
    Touch::Touch(hal::I2cMaster& i2c, services::TouchRecipient& touchRecipient)
        : lcdInt(hal::Port::I, 13)
        , touch(i2c, lcdInt, touchRecipient)
    {}
}
