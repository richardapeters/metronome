#ifndef METRONOME_VIEW_TIMELINE_HPP
#define METRONOME_VIEW_TIMELINE_HPP

#include "infra/util/BoundedVector.hpp"
#include "metronome/interfaces/Notes.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/interfaces/BitmapPainter.hpp"
#include "preview/interfaces/View.hpp"

//#define SHOW_PITCH

namespace application
{
    class ViewTimeline
        : public services::View
        , public NotesObserver
    {
    public:
        ViewTimeline(Notes& notes, hal::BitmapPainter& painter);

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
        virtual void ViewRegionChanged() override;

        // Implementation of NotesObserver
        virtual void NotesChanged(infra::MemoryRange<const Note> newNotes) override;

    private:
        std::pair<uint8_t, const infra::Bitmap*> PitchToDistanceAndBitmap(uint8_t pitch) const;

    private:
        infra::BoundedVector<std::pair<infra::Point, infra::Point>>::WithMaxSize<16> lines;
        infra::BoundedVector<std::pair<infra::Point, const infra::Bitmap*>>::WithMaxSize<128> notes;

        infra::Bitmap::Rgb565<5, 3> noteBase;
        infra::Bitmap::Rgb565<3, 3> noteSnare;
        infra::Bitmap::Rgb565<3, 3> noteTom;
        infra::Bitmap::Rgb565<3, 3> noteCymbal;

#ifdef SHOW_PITCH
        infra::Optional<uint8_t> lastPitch;
#endif
    };
}

#endif
