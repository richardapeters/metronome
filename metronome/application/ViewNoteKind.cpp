#include "metronome/application/ViewNoteKind.hpp"
#include "metronome/application/NoteFont.hpp"

namespace application
{
    ViewNoteKind::ViewNoteKind()
        : services::TouchSpinInteger::WithViewFadingText::WithStorage<1>(2, 0, 4, false, 60, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::lightGray, noteFont })
    {}

    void ViewNoteKind::Report(int32_t steps, services::Direction from)
    {
        services::TouchSpinInteger::WithViewFadingText::WithStorage<1>::Report(steps, from);

        if (previousValue != Value())
            NotifyObservers([this](ViewNoteKindObserver& observer) { observer.SelectedNoteKind(Value()); });

        previousValue = Value();
    }
}
