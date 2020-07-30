#ifndef METRONOME_VIEW_NOTE_KIND_HPP
#define METRONOME_VIEW_NOTE_KIND_HPP

#include "preview/touch/TouchSpinInteger.hpp"

namespace application
{
    class ViewNoteKind
        : public services::TouchSpinInteger::WithViewFadingText::WithStorage<1>
    {
    public:
        ViewNoteKind();
    };
}

#endif
