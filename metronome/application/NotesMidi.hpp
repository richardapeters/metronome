#ifndef METRONOME_NOTES_MIDI_HPP
#define METRONOME_NOTES_MIDI_HPP

#include "hal/interfaces/SerialCommunication.hpp"
#include "metronome/application/BeatController.hpp"
#include "metronome/application/ViewBpm.hpp"

namespace application
{
    class NotesMidi
        : public Notes
        , public BeatTimerObserver
    {
    public:
        NotesMidi(hal::SerialCommunication& serial, MetronomeBeatTimer& beatTimer);

        // Implementation of BeatTimerObserver
        virtual void Beat() override;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> newBeatsPerMeasure) override;
        virtual void Stopped() override;

    private:
        void ReceivedByte(uint8_t byte);
        uint16_t Now() const;
        void AddNote(Note note);

    private:
        enum class State: uint8_t
        {
            initial,
            receivedNoteOn,
            receivedPitch
        } state{ State::initial };

        infra::BoundedVector<Note>::WithMaxSize<256> notes;

        infra::TimePoint beatStart;
        infra::Duration beatDuration;
        uint8_t beatsPerMeasure;
        uint8_t beatInMeasure;
        bool running = false;
    };
}

#endif
