#ifndef METRONOME_SAI_HPP
#define METRONOME_SAI_HPP

#include "hal_st/stm32fxxx/DmaStm.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "local_hal/stm32fxxx/SaiStm.hpp"

namespace main_
{
    struct Sai
    {
        Sai(hal::DmaStm& dma);

        hal::GpioPinStm mclock{ hal::Port::I, 4 };
        hal::GpioPinStm sck{ hal::Port::I, 5 };
        hal::GpioPinStm sd{ hal::Port::I, 6 };
        hal::GpioPinStm fs{ hal::Port::I, 7 };
        hal::SaiStm controller;

    private:
        void ConfigureSaiClock();
    };
}

#endif
