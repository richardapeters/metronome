#include "generated/stm32fxxx/PinoutTableDefault.hpp"
#include "hal_st/stm32fxxx/SystemTickTimerService.hpp"
#include "infra/event/EventDispatcherWithWeakPtr.hpp"
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
    std::array<char, 4> fmt;
    uint32_t formatLength;
    uint16_t formatType;
    uint16_t numberOfChannels;
    uint32_t samplesPerSecond;
    uint32_t bytesPerSecond;
    uint16_t bla;
    uint16_t bitsPerSample;
    uint32_t dummy;
    std::array<char, 4> data;
    uint32_t dataLength;
};

bool StringArrayEqual(infra::MemoryRange<const char> array, infra::BoundedConstString string)
{
    return infra::BoundedConstString(array.begin(), array.size()) == string;
}

infra::ConstByteRange ReadClick(const WavHeader header, infra::ConstByteRange data)
{
    really_assert(StringArrayEqual(header.riff, "RIFF"));
    really_assert(StringArrayEqual(header.wave, "WAVE"));
    really_assert(StringArrayEqual(header.fmt, "fmt "));
    really_assert(StringArrayEqual(header.data, "data"));
    really_assert(header.formatType == 1);
    really_assert(header.numberOfChannels == 1);
    really_assert(header.bitsPerSample == 16);

    return infra::Head(infra::DiscardHead(data, sizeof(header)), header.dataLength);
}

extern WavHeader click_accent_start;
extern uint8_t click_accent_end;

extern WavHeader click_start;
extern uint8_t click_end;

//infra::MemoryRange<const uint16_t> clickAccent(infra::ReinterpretCastMemoryRange<const uint16_t>(ReadClick(click_accent_start, { reinterpret_cast<const uint8_t*>(&click_accent_start), &click_accent_end })));
infra::MemoryRange<const uint16_t> clickAccent(infra::ReinterpretCastMemoryRange<const uint16_t>(ReadClick(click_start, { reinterpret_cast<const uint8_t*>(&click_start), reinterpret_cast<const uint8_t*>(&click_start) + std::distance( reinterpret_cast<const uint8_t*>(&click_start),  reinterpret_cast<const uint8_t*>(&click_end)) / 4 })));
infra::MemoryRange<const uint16_t> click(infra::ReinterpretCastMemoryRange<const uint16_t>(ReadClick(click_start, { reinterpret_cast<const uint8_t*>(&click_start), &click_end })));

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

    static application::Wm8994 wm8994(peripheralI2c.i2cAudio, []()
    {
        static application::MetronomeBeatTimerStm beatTimer(sai.controller, clickAccent, click);
        static hal::BitmapPainterStm bitmapPainter;
        static main_::Metronome metronome(lcd.lcd.ViewingBitmap().size, rtc.rtc, beatTimer, lcd.lcd, bitmapPainter);
        static main_::Touch touch(peripheralI2c.i2cTouch, metronome.touch);
    });

    eventDispatcher.Run();
    __builtin_unreachable();
}
