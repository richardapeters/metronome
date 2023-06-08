#ifndef METRONOME_NOTES_MIDI_HPP
#define METRONOME_NOTES_MIDI_HPP

#include "hal/interfaces/SerialCommunication.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/util/BoundedVector.hpp"
#include "metronome/interfaces/BeatTimer.hpp"
#include "metronome/interfaces/Notes.hpp"

namespace application
{
    class NotesMidi
        : public Notes
        , public BeatTimerObserver
    {
    public:
        NotesMidi(hal::SerialCommunication& serial, BeatTimer& beatTimer);

        // Implementation of BeatTimerObserver
        virtual void Beat(uint8_t subDivision, bool gapped) override;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> newBeatsPerMeasure) override;
        virtual void Stopped() override;

    private:
        void ReceivedByte(uint8_t byte);
        uint16_t Now() const;

    private:
        enum class State : uint8_t
        {
            initial,
            receivedNoteOn,
            receivedPitch
        } state{ State::initial };

        infra::TimePoint beatStart;
        infra::Duration beatDuration;
        uint8_t beatsPerMeasure{ 4 };
        uint8_t beatInMeasure;
        bool running = false;
    };
}

#endif
