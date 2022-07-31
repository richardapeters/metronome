#ifndef METRONOME_NOTES_HPP
#define METRONOME_NOTES_HPP

#include "infra/util/MemoryRange.hpp"
#include "infra/util/Observer.hpp"

namespace application
{
    struct Note
    {
        uint16_t moment;
        uint8_t pitch;
    };

    class Notes;

    class NotesObserver
        : public infra::SingleObserver<NotesObserver, Notes>
    {
    public:
        using infra::SingleObserver<NotesObserver, Notes>::SingleObserver;

        virtual void NoteAdded(Note newNote) = 0;
    };

    class Notes
        : public infra::Subject<NotesObserver>
    {};
}

#endif
