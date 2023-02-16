#include "hal_st/synchronous_stm32fxxx/SynchronousUartStm.hpp"
#include "services/tracer/StreamWriterOnSynchronousSerialCommunication.hpp"
#include "services/tracer/TracerWithDateTime.hpp"

namespace main_
{
    class TimeKeeperStub
        : public hal::TimeKeeper
    {
    public:
        virtual bool Timeout() override
        {
            return false;
        }

        virtual void Reset() override
        {}
    };

    struct Tracer
    {
        Tracer();

        hal::GpioPinStm uartTx;
        hal::GpioPinStm uartRx;
        TimeKeeperStub timeKeeper;
        hal::SynchronousUartStm::WithStorage<10> uart;

        services::StreamWriterOnSynchronousSerialCommunication streamWriter{ uart };
        infra::TextOutputStream::WithErrorPolicy stream{ streamWriter };
        services::TracerWithDateTime tracer{ stream };
    };
}
