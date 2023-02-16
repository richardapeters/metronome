#include "metronome/main_stm32f746/Flash.hpp"

namespace main_
{
    Flash::Flash(hal::DmaStm& dma, infra::Function<void()> onInitialized)
        : quadSpiClock(hal::Port::B, 2)
        , quadSpiChipSelect(hal::Port::B, 6)
        , quadSpiData0(hal::Port::D, 11)
        , quadSpiData1(hal::Port::D, 12)
        , quadSpiData2(hal::Port::E, 2)
        , quadSpiData3(hal::Port::D, 13)
        , quadSpi(dma, quadSpiClock, quadSpiChipSelect, quadSpiData0, quadSpiData1, quadSpiData2, quadSpiData3)
        , flash(quadSpi, onInitialized)
    {}
}
