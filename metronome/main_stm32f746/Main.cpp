#include "generated/stm32fxxx/PinoutTableDefault.hpp"
#include "hal_st/stm32fxxx/SystemTickTimerService.hpp"
#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "local_hal/stm32fxxx/SaiStm.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/Wm8994.hpp"
#include "metronome/main_stm32f746/Flash.hpp"
#include "metronome/main_stm32f746/Lcd.hpp"
#include "metronome/main_stm32f746/LowPowerTimer.hpp"
#include "metronome/main_stm32f746/PeripheralI2c.hpp"
#include "metronome/main_stm32f746/Rtc.hpp"
#include "metronome/main_stm32f746/Touch.hpp"
#include "metronome/main_stm32f746/Tracer.hpp"
#include "preview/interfaces/ViewPainterDoubleBufferDisplay.hpp"
#include "preview/stm32fxxx/BitmapPainterStm.hpp"
#include "services/tracer/GlobalTracer.hpp"
#include "services/util/DebouncedButton.hpp"
#include "services/util/DebugLed.hpp"
#include "services/util/TimeWithLocalization.hpp"

uint32_t GetHseValue()
{
    return 25000000;
}

extern "C" void Default_Handler()
{
    hal::InterruptTable::Instance().Invoke(hal::ActiveInterrupt());
}

//void ContinuouslyPaint(application::ViewPainterMetronome& viewPainter)
//{
//    //auto start = infra::Now();
//    viewPainter.Paint();
//    //auto duration = infra::Now() - start;
//    //services::GlobalTracer().Trace() << "Paint duration: " << std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
//    viewPainter.SwapLayers([&viewPainter]()
//    {
//        static infra::TimerSingleShot repaintTimer;
//        repaintTimer.Start(std::chrono::milliseconds(50), [&viewPainter]() { ContinuouslyPaint(viewPainter); });
//    });
//}

void ConfigureSaiClock()
{
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

    // Configure PLLSAI prescalers
    // PLLI2S_VCO: VCO_429M
    // SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
    // SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 214.5/19 = 11.289 Mhz
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
    RCC_ExCLKInitStruct.PLLI2SDivQ = 19;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
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

extern WavHeader click_start;
extern uint8_t click_end;

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

infra::MemoryRange<const uint16_t> click(infra::ReinterpretCastMemoryRange<const uint16_t>(ReadClick(click_start, { reinterpret_cast<const uint8_t*>(&click_start), &click_end })));

class SaiBeatTimer
    : public application::BeatTimer
    , private hal::LowPowerTimer
{
public:
    SaiBeatTimer(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> data);

    virtual void Start(uint16_t bpm) override;
    virtual void Stop() override;

private:
    virtual void Reload() override;

private:
    hal::SaiStm& sai;
    infra::MemoryRange<const uint16_t> data;
};

SaiBeatTimer::SaiBeatTimer(hal::SaiStm& sai, infra::MemoryRange<const uint16_t> data)
    : sai(sai)
    , data(data)
{}

void SaiBeatTimer::Start(uint16_t bpm)
{
    SetReload(std::min(32768 * 60 / bpm, 65535));
    hal::LowPowerTimer::Start();
}

void SaiBeatTimer::Stop()
{
    hal::LowPowerTimer::Stop();
}

void SaiBeatTimer::Reload()
{
    sai.Transfer(click);
    hal::LowPowerTimer::Reload();
    GetObserver().Beat();
}

int main()
{
    static hal::InterruptTable::WithStorage<128> interruptTable;
    static infra::EventDispatcherWithWeakPtr::WithSize<50> eventDispatcher;
    static hal::GpioStm gpio(hal::pinoutTableDefaultStm);
    static hal::SystemTickTimerService systemTick;
    ConfigureSaiClock();

    static hal::DmaStm dma;

    static main_::Tracer tracer;
    static main_::Lcd lcd;
    static main_::Flash flash(dma, []() {});

    static main_::Rtc rtc;

    static main_::PeripheralI2c peripheralI2c;

    static hal::GpioPinStm mclock(hal::Port::I, 4);
    static hal::GpioPinStm sck(hal::Port::I, 5);
    static hal::GpioPinStm sd(hal::Port::I, 6);
    static hal::GpioPinStm fs(hal::Port::I, 7);
    static hal::SaiStm sai(2, hal::saiA, mclock, sck, sd, fs, dma);

    static application::Wm8994 wm8994(peripheralI2c.i2cAudio, []()
    {
        static SaiBeatTimer beatTimer(sai, click);
        static hal::BitmapPainterStm bitmapPainter;
        static main_::Metronome metronome(lcd.lcd.ViewingBitmap().size, rtc.rtc, beatTimer, lcd.lcd, bitmapPainter);
        static main_::Touch touch(peripheralI2c.i2cTouch, metronome.touch);
    });

    eventDispatcher.Run();
    __builtin_unreachable();
}
