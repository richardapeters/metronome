#ifndef METRONOME_STUCK_I2C_RELEASER_HPP
#define METRONOME_STUCK_I2C_RELEASER_HPP

#include "hal/interfaces/Gpio.hpp"
#include "hal/interfaces/I2c.hpp"
#include "infra/timer/TimerAlternating.hpp"
#include "infra/util/PolymorphicVariant.hpp"

namespace services
{
    class StuckI2cReleaser
        : public hal::I2cMaster
    {
    public:
        StuckI2cReleaser(hal::GpioPin& scl, hal::GpioPin& sda, const infra::Function<hal::I2cMaster&()>& onDone);

    private:
        virtual void SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent) override;
        virtual void ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived) override;

    private:
        class Base
            : public hal::I2cMaster
        {
        public:
            virtual ~Base() = default;
        };

        class Unstucking
            : public Base
        {
        public:
            Unstucking(StuckI2cReleaser& releaser, hal::GpioPin& scl, hal::GpioPin& sda);

        public:
            virtual void SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent) override;
            virtual void ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived) override;

        private:
            class Request
            {
            public:
                virtual ~Request() = default;

                virtual void Execute(hal::I2cMaster& i2c);
            };

            class SendRequest
                : public Request
            {
            public:
                SendRequest(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent);

                virtual void Execute(hal::I2cMaster& i2c) override;

            private:
                hal::I2cAddress address;
                infra::ConstByteRange data;
                hal::Action nextAction;
                infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent;
            };

            class ReceiveRequest
                : public Request
            {
            public:
                ReceiveRequest(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived);

                virtual void Execute(hal::I2cMaster& i2c) override;

            private:
                hal::I2cAddress address;
                infra::ByteRange data;
                hal::Action nextAction;
                infra::Function<void(hal::Result)> onReceived;
            };

        private:
            void Release();
            void Activate();

        private:
            StuckI2cReleaser& releaser;
            hal::TriStatePin scl;
            hal::InputPin sda;
            infra::TimerAlternating toggling;
            infra::PolymorphicVariant<Request, Request, SendRequest, ReceiveRequest> request;
        };

        class Unstucked
            : public Base
        {
        public:
            Unstucked(StuckI2cReleaser& releaser);

            virtual void SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction, infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent) override;
            virtual void ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction, infra::Function<void(hal::Result)> onReceived) override;

        private:
            hal::I2cMaster& i2c;
        };

    private:
        void Done();

    private:
        infra::Function<hal::I2cMaster&()> onDone;
        infra::PolymorphicVariant<Base, Unstucking, Unstucked> state;
    };
}

#endif
