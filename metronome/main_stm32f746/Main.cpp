#include "generated/clicks/Click.hpp"
#include "generated/clicks/ClickAccent.hpp"
#include "generated/stm32fxxx/PinoutTableDefault.hpp"
#include "hal_st/stm32fxxx/SystemTickTimerService.hpp"
#include "hal_st/stm32fxxx/UartStmDma.hpp"
#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "metronome/application/Wm8994.hpp"
#include "metronome/instantiations/Metronome.hpp"
#include "metronome/main_stm32f746/Flash.hpp"
#include "metronome/main_stm32f746/Lcd.hpp"
#include "metronome/main_stm32f746/MetronomeBeatTimerStm.hpp"
#include "metronome/main_stm32f746/PeripheralI2c.hpp"
#include "metronome/main_stm32f746/Rtc.hpp"
#include "metronome/main_stm32f746/Sai.hpp"
#include "metronome/main_stm32f746/SdStm.hpp"
#include "metronome/main_stm32f746/Touch.hpp"
#include "metronome/main_stm32f746/Tracer.hpp"
#include "preview/stm32fxxx/BitmapPainterStm.hpp"
#include "services/tracer/GlobalTracer.hpp"
#include "services/util/DebugLed.hpp"

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

infra::MemoryRange<const int16_t> clickAccent(ReadClick(clicks::clickAccent));
infra::MemoryRange<const int16_t> click(ReadClick(clicks::click));

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
        static hal::GpioPinStm midiRx(hal::Port::C, 7);
        static hal::UartStmDma::Config config;
        config.baudrate = 31250;
        static hal::UartStmDma midi(dma, 6, hal::dummyPinStm, midiRx, config);

        static application::MetronomeBeatTimerStm beatTimer(sai.controller, clickAccent, click, softClick);
        static hal::BitmapPainterStm bitmapPainter;
        static main_::Metronome metronome(lcd.lcd.DisplaySize(), rtc.rtc, beatTimer, lcd.lcd, bitmapPainter, midi, lcd.bitmap0, lcd.bitmap1, lcd.beatBitmap);
        static main_::Touch touch(peripheralI2c.i2cTouch, metronome.touch); });

    // Detect SD card
    /*
        hal::GpioPinStm sdClk(hal::Port::C, 12);
        hal::GpioPinStm sdCmd(hal::Port::D, 2);
        hal::GpioPinStm sdD0(hal::Port::C, 8);
        hal::GpioPinStm sdD1(hal::Port::C, 9);
        hal::GpioPinStm sdD2(hal::Port::C, 10);
        hal::GpioPinStm sdD3(hal::Port::C, 11);
        static hal::SdStm sd(1, sdClk, sdCmd, sdD0, sdD1, sdD2, sdD3, []()
        {
            static std::array<uint8_t, 512> data;
            sd.ReadBuffer(data, 0, []()
            {
                __BKPT();
            });
        });
    */
    eventDispatcher.Run();
    __builtin_unreachable();
}
