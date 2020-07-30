#include "metronome/main_stm32f746/Tracer.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace main_
{
    Tracer::Tracer()
        : uartTx(hal::Port::A, 9)
        , uartRx(hal::Port::B, 7)
        , uart(1, uartTx, uartRx, timeKeeper)
    {
        services::SetGlobalTracerInstance(tracer);
        tracer.Trace() << "----------------------------------------------------------";
    }
}
