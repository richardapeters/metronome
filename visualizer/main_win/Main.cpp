#include "hal/generic/TimerServiceGeneric.hpp"
#include "infra/event/LowPowerEventDispatcher.hpp"
#include "preview/sdl/DirectDisplaySdl.hpp"
#include "preview/sdl/LowPowerStrategySdl.hpp"
#include <Windows.h>

#include "preview/interfaces/View.hpp"

class VisualizationSystem
    : public services::View
{
public:
    virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
};

void VisualizationSystem::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
{

}

#include "hal/interfaces/Dac.hpp"
#include "hal/interfaces/Spi.hpp"
#include "infra/event/ClaimableResource.hpp"

namespace services
{
    class BH2226F
    {
    public:
        BH2226F(hal::SpiMaster& spi);   // Requires a SPI bus that has 12 bit data

        hal::DacImplBase& Output(int oneBasedIndex);

    private:
        class OutputImpl
            : public hal::DacImplBase
        {
        public:
            OutputImpl(BH2226F& controller, int index);

            virtual void SetOutput(uint16_t value) override;

        private:
            BH2226F& controller;
            uint8_t index;
            uint16_t command;

            infra::ClaimableResource::Claimer claimer;
        };

    private:
        hal::SpiMaster& spi;
        std::array<OutputImpl, 8> outputs;

        infra::ClaimableResource resource;
    };

    BH2226F::BH2226F(hal::SpiMaster& spi)
        : spi(spi)
        , outputs{{ { *this, 1}, {*this, 2}, { *this, 3}, {*this, 4}, { *this, 5}, {*this, 6}, { *this, 7}, {*this, 8} }}
    {}

    hal::DacImplBase& BH2226F::Output(int oneBasedIndex)
    {
        return outputs[oneBasedIndex - 1];
    }

    BH2226F::OutputImpl::OutputImpl(BH2226F& controller, int index)
        : controller(controller)
        , index(index)
        , claimer(controller.resource)
    {}

    void BH2226F::OutputImpl::SetOutput(uint16_t value)
    {
        assert(value < 0x100);
        command = value | (index << 8);

        if (!claimer.IsQueued())
            claimer.Claim([this]()
            {
                controller.spi.SendData(infra::MakeByteRange(command), hal::SpiAction::stop, infra::emptyFunction);
            });
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hal::TimerServiceGeneric timerService;
    services::LowPowerStrategySdl lowPowerStrategy(timerService);
    infra::LowPowerEventDispatcher::WithSize<50> eventDispatcher(lowPowerStrategy);

    hal::DirectDisplaySdl display(infra::Vector(480, 128));

    VisualizationSystem visualizationSystem;
    visualizationSystem.SetViewRegion(infra::Region(infra::Point(), display.Size()));
    visualizationSystem.Paint(display, visualizationSystem.ViewRegion());
    display.PaintingComplete();

    eventDispatcher.Run();

    return 0;
}
