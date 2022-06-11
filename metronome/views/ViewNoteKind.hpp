#ifndef METRONOME_VIEW_NOTE_KIND_HPP
#define METRONOME_VIEW_NOTE_KIND_HPP

#include "infra/util/Observer.hpp"
#include "preview/touch/TouchSpinInteger.hpp"

namespace application
{
    class ViewNoteKind;

    class ViewNoteKindObserver
        : public infra::Observer<ViewNoteKindObserver, ViewNoteKind>
    {
    public:
        using infra::Observer<ViewNoteKindObserver, ViewNoteKind>::Observer;

        virtual void SelectedNoteKind(uint8_t noteKind) = 0;
    };

    class ViewNoteKind
        : public services::TouchSpinInteger::WithViewFadingText::WithStorage<1>
        , public infra::Subject<ViewNoteKindObserver>
    {
    public:
        ViewNoteKind();

    protected:
        virtual void Report(int32_t steps, services::Direction from) override;
    };
}

#endif
