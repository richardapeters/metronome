#ifndef METRONOME_VIEW_TIMELINE_HPP
#define METRONOME_VIEW_TIMELINE_HPP

#include "infra/util/BoundedVector.hpp"
#include "metronome/interfaces/Notes.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/interfaces/View.hpp"

//#define SHOW_PITCH

namespace application
{
    class ViewTimeline
        : public services::View
        , public NotesObserver
    {
    public:
        using NotesObserver::NotesObserver;

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
        virtual void ViewRegionChanged() override;

        // Implementation of NotesObserver
        virtual void NotesChanged(infra::MemoryRange<const Note> newNotes) override;

    private:
        uint8_t PitchToDistance(uint8_t pitch) const;

    private:
        infra::BoundedVector<std::pair<infra::Point, infra::Point>>::WithMaxSize<16> lines;
        infra::BoundedVector<infra::Region>::WithMaxSize<128> notes;

#ifdef SHOW_PITCH
        infra::Optional<uint8_t> lastPitch;
#endif
    };
}

#endif
