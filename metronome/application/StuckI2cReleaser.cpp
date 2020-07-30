#include "metronome/application/StuckI2cReleaser.hpp"

namespace services
{
    StuckI2cReleaser::StuckI2cReleaser(hal::GpioPin& scl, hal::GpioPin& sda, const infra::Function<hal::I2cMaster&()>& onDone)
        : onDone(onDone)
        , state(infra::InPlaceType<Unstucking>(), *this, scl, sda)
    {}

    void StuckI2cReleaser::SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent)
    {
        state->SendData(address, data, nextAction, onSent);
    }

    void StuckI2cReleaser::Done()
    {
        state.Emplace<Unstucked>(*this);
    }

    void StuckI2cReleaser::ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived)
    {
        state->ReceiveData(address, data, nextAction, onReceived);
    }

    StuckI2cReleaser::Unstucking::Unstucking(StuckI2cReleaser& releaser, hal::GpioPin& scl, hal::GpioPin& sda)
        : releaser(releaser)
        , scl(scl, false)
        , sda(sda)
        , toggling(std::chrono::milliseconds(1), [this]() { Release(); }, std::chrono::milliseconds(1), [this]() { Activate(); })
    {}

    void StuckI2cReleaser::Unstucking::SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent)
    {
        request.Emplace<SendRequest>(address, data, nextAction, onSent);
    }

    void StuckI2cReleaser::Unstucking::ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived)
    {
        request.Emplace<ReceiveRequest>(address, data, nextAction, onReceived);
    }

    void StuckI2cReleaser::Unstucking::Release()
    {
        scl.SetAsInput();
        if (sda.Get())
        {
            auto requestCopy = request;
            auto& releaserCopy = releaser;
            releaser.Done();
            requestCopy->Execute(releaserCopy);
        }
    }

    void StuckI2cReleaser::Unstucking::Activate()
    {
        scl.Set(false);
    }

    void StuckI2cReleaser::Unstucking::Request::Execute(hal::I2cMaster& i2c)
    {}

    StuckI2cReleaser::Unstucking::SendRequest::SendRequest(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent)
        : address(address)
        , data(data)
        , nextAction(nextAction)
        , onSent(onSent)
    {}

    void StuckI2cReleaser::Unstucking::SendRequest::Execute(hal::I2cMaster& i2c)
    {
        i2c.SendData(address, data, nextAction, onSent);
    }

    StuckI2cReleaser::Unstucking::ReceiveRequest::ReceiveRequest(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived)
        : address(address)
        , data(data)
        , nextAction(nextAction)
        , onReceived(onReceived)
    {}

    void StuckI2cReleaser::Unstucking::ReceiveRequest::Execute(hal::I2cMaster& i2c)
    {
        i2c.ReceiveData(address, data, nextAction, onReceived);
    }

    StuckI2cReleaser::Unstucked::Unstucked(StuckI2cReleaser& releaser)
        : i2c(releaser.onDone())
    {}

    void StuckI2cReleaser::Unstucked::SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent)
    {
        i2c.SendData(address, data, nextAction, onSent);
    }

    void StuckI2cReleaser::Unstucked::ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived)
    {
        i2c.ReceiveData(address, data, nextAction, onReceived);
    }
}
