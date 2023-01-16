#ifndef SAI_STM_HPP
#define SAI_STM_HPP

#include "generated/stm32fxxx/PeripheralTable.hpp"
#include "hal_st/stm32fxxx/DmaStm.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#if defined(HAS_PERIPHERAL_SAI)

namespace hal
{
    struct SaiA
    {};

    struct SaiB
    {};

    extern SaiA saiA;
    extern SaiB saiB;

    class SaiStm
    {
    public:
        SaiStm(uint8_t oneBasedIndex, SaiA, GpioPinStm& mclock, GpioPinStm& sck, GpioPinStm& sd, GpioPinStm& fs, DmaStm& dma);
        SaiStm(uint8_t oneBasedIndex, SaiB, GpioPinStm& mclock, GpioPinStm& sck, GpioPinStm& sd, GpioPinStm& fs, DmaStm& dma);
        ~SaiStm();

        void Transfer(infra::MemoryRange<const int16_t> data);

    private:
        void Init();

    private:
        uint8_t instance;
        PeripheralPinStm mclock;
        PeripheralPinStm sck;
        PeripheralPinStm sd;
        PeripheralPinStm fs;

        SAI_Block_TypeDef* saiBlockHandle;
        hal::DmaStm::Stream tx;
    };
}

#endif

#endif
