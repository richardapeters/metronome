#ifndef METRONOME_BEAT_CONTROLLER_IMPL_HPP
#define METRONOME_BEAT_CONTROLLER_IMPL_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/Optional.hpp"
#include "metronome/interfaces/BeatController.hpp"
#include "metronome/interfaces/BeatTimer.hpp"
#include "metronome/interfaces/MetronomePainter.hpp"
#include <atomic>

namespace application
{
    class BeatControllerImpl
        : public BeatController
        , public MetronomePainterSubject
    {
    public:
        BeatControllerImpl(BeatTimer& beatTimer);

        virtual void SetBpm(uint16_t newBpm) override;
        virtual uint16_t MakeBpmValid(uint16_t bpm) const override;
        virtual bool BpmIsValid(uint16_t bpm) const override;
        virtual void Start() override;
        virtual void Stop() override;
        virtual bool Running() const override;
        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) override;
        virtual void DisabledBeatsPerMinute() override;
        virtual void SelectedNoteKind(uint8_t noteKind) override;

    private:
        BeatTimer& beatTimer;
        uint16_t bpm;
        infra::Optional<uint8_t> beatsPerMeasure;
        uint8_t noteKind = 0;
        bool running = false;

        infra::Duration expectedPaintDuration = std::chrono::milliseconds(120);
    };
}

#endif
