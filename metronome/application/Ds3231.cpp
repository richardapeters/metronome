#include "metronome/application/Ds3231.hpp"
#include "infra/timer/PartitionedTime.hpp"
#include "infra/util/BitLogic.hpp"

namespace services
{
    namespace
    {
        const hal::I2cAddress address{ 0x68 };

        const uint8_t addressStart = 0x00;
        const uint8_t addressControl = 0x0e;
        const uint8_t addressStatus = 0x0f;
        const uint8_t resetOscillatorStopFlag = 0x08;
    }

    Ds3231::Ds3231(hal::I2cMaster& i2c, const infra::Function<void(infra::Optional<infra::TimePoint> time)>& onInitialized)
        : i2c(i2c)
        , onInitialized(onInitialized)
    {
        Initialize();
    }

    void Ds3231::ReadTime(const infra::Function<void(infra::TimePoint time)>& onDone)
    {
        onReadDone = onDone;

        i2c.SendData(address, infra::MakeByteRange(addressStart), hal::Action::repeatedStart, [this](hal::Result, uint32_t numberOfBytesSent)
            { i2c.ReceiveData(address, infra::MakeByteRange(timeData), hal::Action::stop, [this](hal::Result)
                  { onReadDone(Convert(timeData)); }); });
    }

    void Ds3231::SetTime(infra::TimePoint time, const infra::Function<void(infra::TimePoint)>& onDone)
    {
        onSetDone = onDone;
        timeData = Convert(time);

        i2c.SendData(address, infra::MakeByteRange(addressStart), hal::Action::continueSession, [this](hal::Result, uint32_t numberOfBytesSent)
            { i2c.SendData(address, infra::MakeByteRange(timeData), hal::Action::stop, [this](hal::Result, uint32_t numberOfBytesSent)
                  { i2c.SendData(address, infra::MakeByteRange(addressStatus), hal::Action::continueSession, [this](hal::Result, uint32_t numberOfBytesSent)
                        { i2c.SendData(address, infra::MakeByteRange(resetOscillatorStopFlag), hal::Action::stop, [this](hal::Result, uint32_t numberOfBytesSent)
                              { onSetDone(Convert(timeData)); }); }); }); });
    }

    void Ds3231::Initialize()
    {
        static const std::array<uint8_t, 2> init{ addressControl, 0x00 };
        i2c.SendData(address, init, hal::Action::stop, [this](hal::Result, uint32_t numberOfBytesSent)
            { i2c.SendData(address, infra::MakeByteRange(addressStatus), hal::Action::repeatedStart, [this](hal::Result, uint32_t numberOfBytesSent)
                  { i2c.ReceiveData(address, infra::MakeByteRange(status), hal::Action::stop, [this](hal::Result)
                        {
                    if (infra::IsBitSet(status, 7))
                        onInitialized(infra::none);
                    else
                        ReadTime([this](infra::TimePoint time) { onInitialized(infra::MakeOptional(time)); }); }); }); });
    }

    infra::TimePoint Ds3231::Convert(TimeData data) const
    {
        return infra::PartitionedTime(FromBcd(data.year) + 2000, FromBcd(data.month & 0x7f), FromBcd(data.date), FromBcd(data.hours & 0x3f), FromBcd(data.minutes), FromBcd(data.seconds)).ToTimePoint();
    }

    uint8_t Ds3231::FromBcd(uint8_t bcd) const
    {
        return (bcd & 0x0f) + (((bcd & 0xf0) >> 4) * 10);
    }

    Ds3231::TimeData Ds3231::Convert(infra::TimePoint time) const
    {
        TimeData result{};

        infra::PartitionedTime partitioned(time);
        result.seconds = ToBcd(partitioned.seconds);
        result.minutes = ToBcd(partitioned.minutes);
        result.hours = ToBcd(partitioned.hours);
        result.date = ToBcd(partitioned.days);
        result.month = ToBcd(partitioned.months);
        result.year = ToBcd(partitioned.years - 2000);

        return result;
    }

    uint8_t Ds3231::ToBcd(uint8_t x) const
    {
        return x % 10 + ((x / 10) << 4);
    }

    Ds3231TimerService::Ds3231TimerService(hal::I2cMaster& i2c, hal::GpioPin& interrupt, uint32_t id)
        : SettableTimerService(id)
        , rtc(i2c, [this](infra::Optional<infra::TimePoint> newTime)
              { OnInitialized(newTime); })
        , interrupt(interrupt)
    {}

    void Ds3231TimerService::SetTime(infra::TimePoint newTime, const infra::Function<void()>& onDone)
    {
        onSetDone = onDone;
        rtc.SetTime(newTime, [this](infra::TimePoint newTime)
            {
            auto oldTime = *time;
            time = newTime;
            EnableInterrupt();
            onSetDone();
            Jumped(oldTime, newTime); });
    }

    infra::TimePoint Ds3231TimerService::Now() const
    {
        return time.ValueOr(infra::PartitionedTime(2000, 1, 1, 0, 0, 0).ToTimePoint());
    }

    infra::Duration Ds3231TimerService::Resolution() const
    {
        return std::chrono::seconds(1);
    }

    void Ds3231TimerService::OnInitialized(infra::Optional<infra::TimePoint> newTime)
    {
        auto oldTime = time.ValueOr(infra::TimePoint());
        time = newTime;

        if (time != infra::none)
        {
            Jumped(oldTime, *newTime);
            EnableInterrupt();
        }
    }

    void Ds3231TimerService::EnableInterrupt()
    {
        if (!interruptEnabled)
            interrupt.EnableInterrupt([this]()
                { SecondElapsed(); },
                hal::InterruptTrigger::risingEdge);
        interruptEnabled = true;
    }

    void Ds3231TimerService::SecondElapsed()
    {
        *time += std::chrono::seconds(1);
        Progressed(*time);
    }
}
