#ifndef METRONOME_BEAT_CONTROLLER_HPP
#define METRONOME_BEAT_CONTROLLER_HPP

#include "infra/util/Observer.hpp"
#include <cstdint>

namespace application
{
    class BeatController;

    class BeatControllerObserver
        : public infra::SingleObserver<BeatControllerObserver, BeatController>
    {
    public:
        using infra::SingleObserver<BeatControllerObserver, BeatController>::SingleObserver;

        virtual void BeatOn() = 0;
        virtual void BeatOff() = 0;
    };

    class BeatController
        : public infra::Subject<BeatControllerObserver>
    {
    public:
        virtual void SetBpm(uint16_t newBpm) = 0;
        virtual uint16_t MakeBpmValid(uint16_t bpm) const = 0;
        virtual bool BpmIsValid(uint16_t bpm) const = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool Running() const = 0;

        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) = 0;
        virtual void DisabledBeatsPerMinute() = 0;
        virtual void SelectedNoteKind(uint8_t noteKind) = 0;
    };
}

#endif