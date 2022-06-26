#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"
#include "metronome/application/NotesMidi.hpp"
#include <algorithm>

namespace application
{
    NotesMidi::NotesMidi(hal::SerialCommunication& serial, BeatTimer& beatTimer)
        : BeatTimerObserver(beatTimer)
    {
        serial.ReceiveData([this](infra::ConstByteRange data)
            {
                for (auto byte : data)
                    ReceivedByte(byte);
            });
    }

    void NotesMidi::Beat()
    {
        auto newBeatStart = infra::Now();
        beatDuration = newBeatStart - beatStart;
        beatStart = newBeatStart;

        ++beatInMeasure;
        if (beatInMeasure == beatsPerMeasure)
            beatInMeasure = 0;

        auto startErase = beatInMeasure * std::numeric_limits<uint16_t>::max() / beatsPerMeasure;
        auto endErase = (beatInMeasure + 1) * std::numeric_limits<uint16_t>::max() / beatsPerMeasure;

        notes.erase(std::remove_if(notes.begin(), notes.end(), [startErase, endErase](auto note) { return note.moment >= startErase && note.moment <= endErase; }), notes.end());
        infra::EventDispatcher::Instance().Schedule([this]() { GetObserver().NotesChanged(infra::MakeRange(notes)); });
    }

    void NotesMidi::Started(uint16_t bpm, infra::Optional<uint8_t> newBeatsPerMeasure)
    {
        beatDuration = infra::Duration(std::chrono::minutes(1)) / bpm;
        beatStart = infra::Now() - beatDuration;
        beatsPerMeasure = newBeatsPerMeasure.ValueOr(4);
        beatInMeasure = beatsPerMeasure - 1;
        running = true;
    }

    void NotesMidi::Stopped()
    {
        running = false;
        notes.clear();
        GetObserver().NotesChanged(infra::MakeRange(notes));
    }

    void NotesMidi::ReceivedByte(uint8_t byte)
    {
        if (infra::IsBitSet(byte, 7))
        {
            state = State::initial;

            if (byte == 0x99)
                state = State::receivedNoteOn;
        }
        else
        {
            if (state == State::receivedNoteOn)
            {
                state = State::receivedPitch;
                Note note{ Now(), byte };
                infra::EventDispatcher::Instance().Schedule([this, note]()
                    {
                        if (running)
                            AddNote(note);
                    });
            }
            else if (state == State::receivedPitch)
                state = State::initial;
        }
    }

    uint16_t NotesMidi::Now() const
    {
        return ((infra::Now() - beatStart) + beatDuration * beatInMeasure) * std::numeric_limits<uint16_t>::max() / beatsPerMeasure / beatDuration;
    }

    void NotesMidi::AddNote(Note note)
    {
        if (notes.full())
            notes.erase(notes.begin());

        notes.push_back(note);

        GetObserver().NotesChanged(infra::MakeRange(notes));
    }
}
