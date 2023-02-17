#include "metronome/main_stm32f746/Lcd.hpp"

namespace main_
{
    Lcd::Lcd()
        : sdRamPins(hal::stm32f7discoveryFmcPins)
        , sdRam(sdRamPins, hal::stm32f7discoverySdRamConfig)
        , lcdPins(hal::stm32f7discoveryLcdPins)
        , displayEnable(hal::Port::I, 12)
        , backlightEnable(hal::Port::K, 3)
        , bufferSize(infra::Bitmap::BufferSize(hal::stm32f7discoveryLcdConfig.width, hal::stm32f7discoveryLcdConfig.height, hal::stm32f7discoveryLcdConfig.pixelFormat))
        , lcdBuffer0(infra::Head(sdRam.Memory(), bufferSize))
        , lcdBuffer1(infra::Head(infra::DiscardHead(sdRam.Memory(), bufferSize), bufferSize))
        , lcdBuffer2(infra::Head(infra::DiscardHead(sdRam.Memory(), 2 * bufferSize), bufferSize))
        , lcd(lcdPins, displayEnable, backlightEnable, lcdBuffer0, lcdBuffer1, hal::stm32f7discoveryLcdConfig)
    {}
}
