#include "metronome/main_stm32f746/Lcd.hpp"

namespace main_
{
    namespace
    {
        const std::array<std::pair<hal::Port, uint8_t>, 39> tableFmc = { {
            { hal::Port::C, 3 },{ hal::Port::D, 0 },{ hal::Port::D, 1 },{ hal::Port::D, 3 },
            { hal::Port::D, 8 },{ hal::Port::D, 9 },{ hal::Port::D, 10 },{ hal::Port::D, 14 },
            { hal::Port::D, 15 },{ hal::Port::E, 0 },{ hal::Port::E, 1 },{ hal::Port::E, 7 },
            { hal::Port::E, 8 },{ hal::Port::E, 9 },{ hal::Port::E, 10 },{ hal::Port::E, 11 },
            { hal::Port::E, 12 },{ hal::Port::E, 13 },{ hal::Port::E, 14 },{ hal::Port::E, 15 },
            { hal::Port::F, 0 },{ hal::Port::F, 1 },{ hal::Port::F, 2 },{ hal::Port::F, 3 },
            { hal::Port::F, 4 },{ hal::Port::F, 5 },{ hal::Port::F, 11 },{ hal::Port::F, 12 },
            { hal::Port::F, 13 },{ hal::Port::F, 14 },{ hal::Port::F, 15 },{ hal::Port::G, 0 },
            { hal::Port::G, 1 },{ hal::Port::G, 4 },{ hal::Port::G, 5 },{ hal::Port::G, 8 },
            { hal::Port::G, 15 },{ hal::Port::H, 3 },{ hal::Port::H, 5 }
            } };

        const std::array<std::pair<hal::Port, uint8_t>, 28> tableLcd = { {
            { hal::Port::E, 4 },{ hal::Port::G, 12 },{ hal::Port::I, 9 },{ hal::Port::I, 10 },
            { hal::Port::I, 14 },{ hal::Port::I, 15 },{ hal::Port::J, 0 },
            { hal::Port::J, 1 },{ hal::Port::J, 2 },{ hal::Port::J, 3 },{ hal::Port::J, 4 },
            { hal::Port::J, 5 },{ hal::Port::J, 6 },{ hal::Port::J, 7 },{ hal::Port::J, 8 },
            { hal::Port::J, 9 },{ hal::Port::J, 10 },{ hal::Port::J, 11 },{ hal::Port::J, 13 },
            { hal::Port::J, 14 },{ hal::Port::J, 15 },{ hal::Port::K, 0 },{ hal::Port::K, 1 },
            { hal::Port::K, 2 },{ hal::Port::K, 4 },{ hal::Port::K, 5 },{ hal::Port::K, 6 },
            { hal::Port::K, 7 },
            } };
    }

    Lcd::Lcd()
        : sdRamPins(tableFmc)
        , sdRam(sdRamPins, hal::stm32f7discoverySdRamConfig)
        , lcdPins(tableLcd)
        , displayEnable(hal::Port::I, 12)
        , backlightEnable(hal::Port::K, 3)
        , bufferSize(infra::Bitmap::BufferSize(hal::stm32f7discoveryLcdConfig.width, hal::stm32f7discoveryLcdConfig.height, hal::stm32f7discoveryLcdConfig.pixelFormat))
        , lcdBuffer0(infra::Head(sdRam.Memory(), bufferSize))
        , lcdBuffer1(infra::Head(infra::DiscardHead(sdRam.Memory(), bufferSize), bufferSize))
        , lcdBuffer2(infra::Head(infra::DiscardHead(sdRam.Memory(), 2 * bufferSize), bufferSize))
        , lcd(lcdPins, displayEnable, backlightEnable, lcdBuffer0, lcdBuffer1, hal::stm32f7discoveryLcdConfig)
    {}
}
