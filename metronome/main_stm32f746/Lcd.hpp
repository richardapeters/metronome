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
        infra::Bitmap bitmap0{ lcdBuffer0, infra::Vector(480, 272), infra::PixelFormat::rgb565 };
        infra::Bitmap bitmap1{ lcdBuffer1, infra::Vector(480, 272), infra::PixelFormat::rgb565 };
        infra::Bitmap beatBitmap{ lcdBuffer2, infra::Vector(480, 272), infra::PixelFormat::rgb565 };
        hal::LcdStmMultiBuffer lcd;
    };
}

#endif
