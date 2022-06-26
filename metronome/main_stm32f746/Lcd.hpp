#ifndef METRONOME_LCD_HPP
#define METRONOME_LCD_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/SdRamStm.hpp"
#include "preview/stm32fxxx/LcdStm.hpp"

namespace main_
{
    struct Lcd
    {
        Lcd();

        hal::MultiGpioPinStm sdRamPins;
        hal::SdRamStm sdRam;

        hal::MultiGpioPinStm lcdPins;
        hal::GpioPinStm displayEnable;
        hal::GpioPinStm backlightEnable;
        uint32_t bufferSize;
        infra::ByteRange lcdBuffer0;
        infra::ByteRange lcdBuffer1;
        infra::ByteRange lcdBuffer2;
        hal::LcdStmDoubleBuffer lcd;
    };
}

#endif
