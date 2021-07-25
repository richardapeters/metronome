#ifndef METRONOME_SD_STM_HPP
#define METRONOME_SD_STM_HPP

#include "hal/interfaces/Flash.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "stm32f7xx_hal_sd.h"

namespace hal
{
    class SdStm
        : public hal::Flash64
    {
    public:
        SdStm(uint8_t oneBasedIndex, hal::GpioPinStm& clock, hal::GpioPinStm& cmd, hal::GpioPinStm& d0, hal::GpioPinStm& d1, hal::GpioPinStm& d2, hal::GpioPinStm& d3, const infra::Function<void()>& onInitialized);
        ~SdStm();

        // Implementation of Flash64
        virtual uint64_t NumberOfSectors() const override;
        virtual uint32_t SizeOfSector(uint64_t sectorIndex) const override;
        virtual uint64_t SectorOfAddress(uint64_t address) const override;
        virtual uint64_t AddressOfSector(uint64_t sectorIndex) const override;
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint64_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint64_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint64_t beginIndex, uint64_t endIndex, infra::Function<void()> onDone) override;

    private:
        uint8_t instance;
        hal::PeripheralPinStm clock;
        hal::PeripheralPinStm cmd;
        hal::PeripheralPinStm d0;
        hal::PeripheralPinStm d1;
        hal::PeripheralPinStm d2;
        hal::PeripheralPinStm d3;

        SD_HandleTypeDef handle{};
        HAL_SD_CardInfoTypedef cardInfo{};

        uint64_t numberOfSectors = 0;
        uint32_t sizeOfEachSector = 0;
    };
}

#endif
