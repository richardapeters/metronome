#include "generated/stm32fxxx/PeripheralTable.hpp"
#include "local_hal/stm32fxxx/SaiStm.hpp"
#include <cassert>

#if defined(HAS_PERIPHERAL_SAI)

namespace hal
{
    SaiA saiA;
    SaiB saiB;

    SaiStm::SaiStm(uint8_t oneBasedinstance, SaiA, GpioPinStm& mclock, GpioPinStm& sck, GpioPinStm& sd, GpioPinStm& fs, DmaStm& dma)
        : instance(oneBasedinstance - 1)
        , mclock(mclock, PinConfigTypeStm::saiMClock, oneBasedinstance)
        , sck(sck, PinConfigTypeStm::saiSck, oneBasedinstance)
        , sd(sd, PinConfigTypeStm::saiSd, oneBasedinstance)
        , fs(fs, PinConfigTypeStm::saiFs, oneBasedinstance)
        , saiBlockHandle(reinterpret_cast<SAI_Block_TypeDef*>(peripheralSai[instance] + 1))
        , tx(dma, DmaChannelId{ 2, 4, 3 }, &saiBlockHandle->DR, [this]() {})    // SAI 2A
    {
        Init();
    }

    SaiStm::SaiStm(uint8_t oneBasedinstance, SaiB, GpioPinStm& mclock, GpioPinStm& sck, GpioPinStm& sd, GpioPinStm& fs, DmaStm& dma)
        : instance(oneBasedinstance - 1)
        , mclock(mclock, PinConfigTypeStm::saiMClock, oneBasedinstance)
        , sck(sck, PinConfigTypeStm::saiSck, oneBasedinstance)
        , sd(sd, PinConfigTypeStm::saiSd, oneBasedinstance)
        , fs(fs, PinConfigTypeStm::saiFs, oneBasedinstance)
        , saiBlockHandle(reinterpret_cast<SAI_Block_TypeDef*>(peripheralSai[instance] + 1) + 1)
        , tx(dma, DmaChannelId{ 2, 6, 3 }, &saiBlockHandle->DR, [this]() {})    // SAI 2B
    {
        Init();
    }

    SaiStm::~SaiStm()
    {
        DisableClockSai(instance);
    }

    void SaiStm::Transfer(infra::MemoryRange<const int16_t> data)
    {
        tx.StopTransfer();
        tx.StartTransmit(infra::ReinterpretCastByteRange(data));
        saiBlockHandle->CR1 |= SAI_xCR1_DMAEN;
    }

    void SaiStm::Init()
    {
        tx.SetDataSize(2);

        EnableClockSai(instance);

        saiBlockHandle->CR1 = SAI_xCR1_MONO | SAI_xCR1_OUTDRIV | SAI_xCR1_DS_2;
        saiBlockHandle->CR2 = 0;
        saiBlockHandle->FRCR = SAI_xFRCR_FSOFF | SAI_xFRCR_FSDEF | (15 << 8) | 31;
        saiBlockHandle->SLOTR = (3 << 16) | (1 << 8);

        saiBlockHandle->CR1 |= SAI_xCR1_SAIEN;
    }
}

#endif
