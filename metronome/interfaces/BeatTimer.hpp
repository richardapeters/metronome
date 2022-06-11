#ifndef METRONOME_BEAT_TIMER_HPP
#define METRONOME_BEAT_TIMER_HPP

#include "infra/util/Observer.hpp"
#include "infra/util/Optional.hpp"
#include <cstdint>

namespace application
{
    class BeatTimer;

    class BeatTimerObserver
        : public infra::Observer<BeatTimerObserver, BeatTimer>
    {
    public:
        using infra::Observer<BeatTimerObserver, BeatTimer>::Observer;

        virtual void Beat() = 0;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure) = 0;
        virtual void Stopped() = 0;
    };

    class BeatTimer
        : public infra::Subject<BeatTimerObserver>
    {
    public:
        virtual void Start(uint16_t bpm, infra::Optional<uint8_t> beatsPerMeasure, uint8_t noteKind) = 0;
        virtual void Stop() = 0;
    };
}

#endif
