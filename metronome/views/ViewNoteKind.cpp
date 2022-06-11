#include "metronome/views/ViewNoteKind.hpp"
#include "metronome/views/NoteFont.hpp"

namespace application
{
    ViewNoteKind::ViewNoteKind()
        : services::TouchSpinInteger::WithViewFadingText::WithStorage<1>(2, 0, 4, false, 60, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::lightGray, noteFont })
    {}

    void ViewNoteKind::Report(int32_t steps, services::Direction from)
    {
        services::TouchSpinInteger::WithViewFadingText::WithStorage<1>::Report(steps, from);

        NotifyObservers([this](ViewNoteKindObserver& observer) { observer.SelectedNoteKind(Value()); });
    }
}
