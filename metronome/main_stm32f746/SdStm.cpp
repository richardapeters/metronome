#include "generated/stm32fxxx/PeripheralTable.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "metronome/main_stm32f746/SdStm.hpp"

namespace hal
{
    SdStm::SdStm(uint8_t oneBasedIndex, hal::GpioPinStm& clock, hal::GpioPinStm& cmd, hal::GpioPinStm& d0, hal::GpioPinStm& d1, hal::GpioPinStm& d2, hal::GpioPinStm& d3, const infra::Function<void()>& onInitialized)
        : instance(oneBasedIndex - 1)
        , clock(clock, hal::PinConfigTypeStm::sdClk, oneBasedIndex)
        , cmd(cmd, hal::PinConfigTypeStm::sdCmd, oneBasedIndex)
        , d0(d0, hal::PinConfigTypeStm::sdD0, oneBasedIndex)
        , d1(d1, hal::PinConfigTypeStm::sdD1, oneBasedIndex)
        , d2(d2, hal::PinConfigTypeStm::sdD2, oneBasedIndex)
        , d3(d3, hal::PinConfigTypeStm::sdD3, oneBasedIndex)
    {
        hal::EnableClockSd(instance);

        handle.Instance = SDMMC1;

        handle.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
        handle.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
        handle.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
        handle.Init.BusWide = SDMMC_BUS_WIDE_1B;
        handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
        handle.Init.ClockDiv = SDMMC_TRANSFER_CLK_DIV;

        HAL_SD_Init(&handle, &cardInfo);
        HAL_SD_WideBusOperation_Config(&handle, SDMMC_BUS_WIDE_4B);

        numberOfSectors = cardInfo.CardCapacity / cardInfo.CardBlockSize;
        sizeOfEachSector = cardInfo.CardBlockSize;

        infra::EventDispatcher::Instance().Schedule(onInitialized);
    }

    uint64_t SdStm::NumberOfSectors() const
    {
        return numberOfSectors;
    }

    uint32_t SdStm::SizeOfSector(uint64_t sectorIndex) const
    {
        return sizeOfEachSector;
    }

    uint64_t SdStm::SectorOfAddress(uint64_t address) const
    {
        return address / sizeOfEachSector;
    }

    uint64_t SdStm::AddressOfSector(uint64_t sectorIndex) const
    {
        return sectorIndex * sizeOfEachSector;
    }

    void SdStm::WriteBuffer(infra::ConstByteRange buffer, uint64_t address, infra::Function<void()> onDone)
    {
        std::abort();
    }

    void SdStm::ReadBuffer(infra::ByteRange buffer, uint64_t address, infra::Function<void()> onDone)
    {
        really_assert(sizeOfEachSector == 1024);

        auto discard = address % sizeOfEachSector;
        address -= discard;

        while (!buffer.empty())
        {
            std::array<uint32_t, 1024 / 4> data;
            auto result = HAL_SD_ReadBlocks(&handle, data.begin(), address, 512, 1);
            really_assert(result == SD_OK);

            auto byteData = infra::Head(infra::DiscardHead(infra::ReinterpretCastByteRange(infra::MakeRange(data)), discard), buffer.size());
            infra::Copy(byteData, buffer);
            buffer.pop_front(byteData.size());
            address += discard + byteData.size();

            discard = 0;
        }

        infra::EventDispatcher::Instance().Schedule(onDone);
    }

    void SdStm::EraseSectors(uint64_t beginIndex, uint64_t endIndex, infra::Function<void()> onDone)
    {
        std::abort();
    }

    SdStm::~SdStm()
    {
        hal::DisableClockSd(instance);
    }
}
