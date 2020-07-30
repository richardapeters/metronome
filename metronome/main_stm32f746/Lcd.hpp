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
        infra::ByteRange lcdBuffer;
        hal::LcdStm lcd;
    };
}

#endif
