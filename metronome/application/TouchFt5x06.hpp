#ifndef METRONOME_TOUCH_5X06_HPP
#define METRONOME_TOUCH_5X06_HPP

#include "hal/interfaces/Gpio.hpp"
#include "hal/interfaces/I2c.hpp"
#include "preview/interfaces/Geometry.hpp"
#include "Preview/touch/TouchRecipient.hpp"

namespace application
{
    class TouchFt5x06
    {
    public:
        TouchFt5x06(hal::I2cMaster& i2c, hal::GpioPin& interrupt);

    protected:
        virtual void Touched(infra::Point position) = 0;
        virtual void NoTouch() = 0;

    private:
        void Measure();
        void MeasureTouch();

    private:
        hal::I2cMaster& i2c;
        hal::InputPin interrupt;

        bool idle = true;
        uint8_t status;
        uint32_t xy;
    };

    class TouchFt5x06OnTouchRecipient
        : public TouchFt5x06
    {
    public:
        TouchFt5x06OnTouchRecipient(hal::I2cMaster& i2c, hal::GpioPin& interrupt, services::TouchRecipient& touchRecipient);

    protected:
        virtual void Touched(infra::Point position) override;
        virtual void NoTouch() override;

    private:
        services::TouchRecipient& touchRecipient;
        bool touching = false;
    };
}

#endif
