#include "metronome/application/ViewNoteKind.hpp"
#include "metronome/application/NoteFont.hpp"

namespace application
{
    ViewNoteKind::ViewNoteKind()
        : services::TouchSpinInteger::WithViewFadingText::WithStorage<1>(2, 0, 4, false, 60, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::lightGray, noteFont })
    {}
}
