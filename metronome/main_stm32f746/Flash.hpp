#ifndef METRONOME_FLASH_HPP
#define METRONOME_FLASH_HPP

#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/QuadSpiStmDma.hpp"
#include "services/util/FlashQuadSpiMicronN25q.hpp"

namespace main_
{
    struct Flash
    {
        Flash(hal::DmaStm& dma, infra::Function<void()> onInitialized);

        hal::GpioPinStm quadSpiClock;
        hal::GpioPinStm quadSpiChipSelect;
        hal::GpioPinStm quadSpiData0;
        hal::GpioPinStm quadSpiData1;
        hal::GpioPinStm quadSpiData2;
        hal::GpioPinStm quadSpiData3;
        hal::QuadSpiStmDma quadSpi;
        services::FlashQuadSpiMicronN25q flash;
    };
}

#endif
