#include "generated/stm32fxxx/PeripheralTable.hpp"
#include "generated/stm32fxxx/PinoutTableDefault.hpp"
#include "hal_st/stm32fxxx/SystemTickTimerService.hpp"
#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/Wm8994.hpp"
#include "metronome/main_stm32f746/Flash.hpp"
#include "metronome/main_stm32f746/Lcd.hpp"
#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"
#include "metronome/main_stm32f746/PeripheralI2c.hpp"
#include "metronome/main_stm32f746/Rtc.hpp"
#include "metronome/main_stm32f746/Sai.hpp"
#include "metronome/main_stm32f746/Touch.hpp"
#include "metronome/main_stm32f746/Tracer.hpp"
#include "preview/stm32fxxx/BitmapPainterStm.hpp"
#include "services/tracer/GlobalTracer.hpp"
#include "services/util/DebugLed.hpp"
#include "stm32f7xx_hal_sd.h"

uint32_t GetHseValue()
{
    return 25000000;
}

extern "C" void Default_Handler()
{
    hal::InterruptTable::Instance().Invoke(hal::ActiveInterrupt());
}

struct WavHeader
{
    std::array<char, 4> riff;
    uint32_t fileSize;
    std::array<char, 4> wave;
};

struct WavHeaderChunk
{
    std::array<char, 4> subChunkId;
    uint32_t subChunkLength;
};

struct WavHeaderChunkFormat
{
    uint16_t formatType;
    uint16_t numberOfChannels;
    uint32_t samplesPerSecond;
    uint32_t bytesPerSecond;
    uint16_t bla;
    uint16_t bitsPerSample;
};


bool StringArrayEqual(infra::MemoryRange<const char> array, infra::BoundedConstString string)
{
    return infra::BoundedConstString(array.begin(), array.size()) == string;
}

infra::MemoryRange<const int16_t> FirstChannel(infra::MemoryRange<const int16_t> twoChannels)
{
    auto memory = new int16_t[twoChannels.size() / 2];
    infra::MemoryRange<int16_t> result(memory, memory + twoChannels.size() / 2);

    for (auto& r : result)
    {
        r = twoChannels.front();
        twoChannels.pop_front(2);
    }

    return result;
}

infra::MemoryRange<const int16_t> ReadClick(infra::ConstByteRange data)
{
    infra::ByteInputStream stream(data);

    auto header = stream.Extract<WavHeader>();
    really_assert(StringArrayEqual(header.riff, "RIFF"));
    really_assert(StringArrayEqual(header.wave, "WAVE"));

    auto headerFormatChunk = stream.Extract<WavHeaderChunk>();
    really_assert(StringArrayEqual(headerFormatChunk.subChunkId, "fmt "));

    auto headerFormatChunkFormat = stream.Extract<WavHeaderChunkFormat>();
    stream.Reader().ResetRange(infra::DiscardHead(stream.Reader().Remaining(), headerFormatChunk.subChunkLength - sizeof(WavHeaderChunkFormat)));
    really_assert(headerFormatChunkFormat.formatType == 1);
    really_assert(headerFormatChunkFormat.numberOfChannels == 1 || headerFormatChunkFormat.numberOfChannels == 2);
    really_assert(headerFormatChunkFormat.bitsPerSample == 16);

    while (!stream.Empty())
    {
        auto headerDataChunk = stream.Extract<WavHeaderChunk>();
        if (StringArrayEqual(headerDataChunk.subChunkId, "data"))
        {
            if (headerFormatChunkFormat.numberOfChannels == 1)
                return infra::ReinterpretCastMemoryRange<const int16_t>(infra::Head(stream.Reader().Remaining(), headerDataChunk.subChunkLength));
            else
                return FirstChannel(infra::ReinterpretCastMemoryRange<const int16_t>(infra::Head(stream.Reader().Remaining(), headerDataChunk.subChunkLength)));
        }

        stream.Reader().ResetRange(infra::DiscardHead(stream.Reader().Remaining(), headerDataChunk.subChunkLength));
    }

    std::abort();
}

infra::MemoryRange<const int16_t> CreateSoftClick(infra::MemoryRange<const int16_t> click)
{
    auto block = reinterpret_cast<int16_t*>(malloc(click.size() * sizeof(int16_t)));
    infra::MemoryRange<int16_t> result(block, block + click.size());

    for (auto i = 0; i != click.size(); ++i)
        result[i] = click[i] / 2;

    return result;
}

extern WavHeader click_accent_start;
extern uint8_t click_accent_end;

extern WavHeader click_start;
extern uint8_t click_end;

infra::MemoryRange<const int16_t> clickAccent(ReadClick({ reinterpret_cast<const uint8_t*>(&click_accent_start), &click_accent_end }));
infra::MemoryRange<const int16_t> click(ReadClick({ reinterpret_cast<const uint8_t*>(&click_start), &click_end }));

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

    handle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
    handle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
    handle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    handle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
    handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
    handle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

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

int main()
{
    static hal::InterruptTable::WithStorage<128> interruptTable;
    static infra::EventDispatcherWithWeakPtr::WithSize<50> eventDispatcher;
    static hal::GpioStm gpio(hal::pinoutTableDefaultStm);
    static hal::SystemTickTimerService systemTick;

    static hal::DmaStm dma;

    static main_::Tracer tracer;
    static main_::Lcd lcd;
    static main_::Flash flash(dma, []() {});

    static main_::Rtc rtc;
    static main_::PeripheralI2c peripheralI2c;
    static main_::Sai sai(dma);

    static infra::MemoryRange<const int16_t> softClick = CreateSoftClick(click);

    static application::Wm8994 wm8994(peripheralI2c.i2cAudio, []()
    {
        static application::MetronomeBeatTimerStm beatTimer(sai.controller, clickAccent, click, softClick);
        static hal::BitmapPainterStm bitmapPainter;
        static main_::Metronome metronome(lcd.lcd.ViewingBitmap().size, rtc.rtc, beatTimer, lcd.lcd, bitmapPainter);
        static main_::Touch touch(peripheralI2c.i2cTouch, metronome.touch);
    });

    // Detect SD card

    hal::GpioPinStm sdClk(hal::Port::C, 12);
    hal::GpioPinStm sdCmd(hal::Port::D, 2);
    hal::GpioPinStm sdD0(hal::Port::C, 8);
    hal::GpioPinStm sdD1(hal::Port::C, 9);
    hal::GpioPinStm sdD2(hal::Port::C, 10);
    hal::GpioPinStm sdD3(hal::Port::C, 11);
    static SdStm sd(1, sdClk, sdCmd, sdD0, sdD1, sdD2, sdD3, []()
	{
    	static std::array<uint8_t, 512> data;
    	sd.ReadBuffer(data, 0, []()
		{
    		__BKPT();
	    });
	});

    eventDispatcher.Run();
    __builtin_unreachable();
}
