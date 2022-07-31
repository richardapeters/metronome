#ifndef METRONOME_VIEW_TIMELINE_HPP
#define METRONOME_VIEW_TIMELINE_HPP

#include "infra/util/BoundedDeque.hpp"
#include "infra/util/BoundedVector.hpp"
#include "metronome/interfaces/BeatTimer.hpp"
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
        , public BeatTimerObserver
    {
    public:
        ViewTimeline(Notes& notes, hal::BitmapPainter& painter, BeatTimer& beatTimer);

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
        virtual void ViewRegionChanged() override;

        // Implementation of NotesObserver
        virtual void NoteAdded(Note newNote) override;

        // Implementation of BeatTimerObserver
        virtual void Beat() override;
        virtual void Started(uint16_t bpm, infra::Optional<uint8_t> newBeatsPerMeasure) override;
        virtual void Stopped() override;

    private:
        std::pair<uint8_t, const infra::Bitmap*> PitchToDistanceAndBitmap(uint8_t pitch) const;

    private:
        infra::BoundedVector<std::pair<infra::Point, infra::Point>>::WithMaxSize<16> lines;
        infra::BoundedDeque<std::tuple<infra::Point, const infra::Bitmap*, uint16_t>>::WithMaxSize<128> notes;

        infra::Bitmap::Rgb565<5, 5> noteTom;
        infra::Bitmap::Rgb565<5, 5> noteHiHat;
        infra::Bitmap::Rgb565<5, 5> noteCymbal;

        uint8_t beatIndex = 0;

#ifdef SHOW_PITCH
        infra::Optional<uint8_t> lastPitch;
#endif
    };
}

#endif
