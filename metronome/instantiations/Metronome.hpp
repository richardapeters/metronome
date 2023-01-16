#ifndef METRONOME_METRONOME_HPP
#define METRONOME_METRONOME_HPP

#include "metronome/application/BeatControllerImpl.hpp"
#include "metronome/application/BeatPainter.hpp"
#include "metronome/application/BpmSelectionInteractor.hpp"
#include "metronome/application/Ds3231.hpp"
#include "metronome/application/NotesMidi.hpp"
#include "metronome/views/ViewBeatsPerMeasure.hpp"
#include "metronome/views/ViewBpm.hpp"
#include "metronome/views/ViewDateEntry.hpp"
#include "metronome/views/ViewNoteKind.hpp"
#include "metronome/views/ViewStart.hpp"
#include "metronome/views/ViewTapMeasurement.hpp"
#include "preview/interfaces/BitmapPainter.hpp"
#include "preview/interfaces/DoubleBufferDisplay.hpp"
#include "preview/interfaces/ViewPainterDoubleBufferDisplay.hpp"
#include "preview/interfaces/ViewRepainter.hpp"
#include "preview/touch/TouchButton.hpp"
#include "preview/touch/TouchHorizontalLayout.hpp"
#include "preview/touch/TouchPanel.hpp"
#include "preview/touch/TouchVerticalLayout.hpp"
#include "preview/touch/TouchViewMultiple.hpp"
#include "preview/views/ViewButtonPanel.hpp"
#include "preview/views/ViewCurrentTime.hpp"

namespace application
{
    class BeatsPerMeasureInteractor
        : public ViewBeatsPerMeasureObserver
    {
    public:
        BeatsPerMeasureInteractor(ViewBeatsPerMeasure& subject, BeatController& beatController);

        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) override;
        virtual void DisabledBeatsPerMinute() override;

    private:
        BeatController& beatController;
    };

    class NoteKindInteractor
        : public ViewNoteKindObserver
    {
    public:
        NoteKindInteractor(ViewNoteKind& subject, BeatController& beatController);

        virtual void SelectedNoteKind(uint8_t noteKind) override;

    private:
        BeatController& beatController;
    };
}

namespace main_
{
    struct Metronome
    {
        Metronome(infra::Vector size, services::SettableTimerService& localTime, application::BeatTimer& beatTimer,
            hal::MultiBufferDisplay& display, hal::BitmapPainter& bitmapPainter, hal::SerialCommunication& serialMidi, infra::Bitmap& bitmap0, infra::Bitmap& bitmap1, infra::Bitmap& beatBitmap);

        void StartTimeEntry();
        void StopTimeEntry(infra::TimePoint newTime);

        application::BeatControllerImpl beatController;
        application::NotesMidi notes;
        services::TouchViewMultiple::WithMaxViews<2> touch;
        application::BeatPainter beatPainter;
        services::ViewPainterDoubleBufferDisplay viewPainter;
        services::ViewRepainterPaintWhenDirty repainter;
        services::TouchHorizontalLayout::WithMaxViews<2> touchBpm;
        application::ViewBpm viewBpm;
        application::BpmSelectionInteractor bpmSelectionObserver{ viewBpm, beatController };
        services::TouchVerticalLayout::WithMaxViews<3> touchVertical;
        services::TouchHorizontalLayout::WithMaxViews<2> touchHorizontalTop;
        application::ViewStart viewStart{ beatController, infra::Colour::lightGray };
        application::ViewTapMeasurement viewTapMeasurement{ bpmSelectionObserver, infra::Colour::lightGray, infra::Colour::darkGray };
        services::TouchHorizontalLayout::WithMaxViews<2> touchHorizontalMid;
        services::TouchPanel::WithView<application::ViewBeatsPerMeasure> viewBeatsPerMeasure{ infra::Colour::lightGray };
        services::TouchPanel::WithView<application::ViewNoteKind> viewNoteKind{ infra::Colour::lightGray };
        services::TouchButton::With<services::ViewButtonPanel::WithView<services::ViewCurrentTime>> viewCurrentTime;
        application::BeatsPerMeasureInteractor beatsPerMeasureInteractor{ viewBeatsPerMeasure.SubView(), beatController };
        application::NoteKindInteractor noteKindInteractor{ viewNoteKind.SubView(), beatController };

        services::SettableTimerService& localTime;
        infra::Optional<services::TouchAligned::WithView<application::ViewDateEntry>> dateEntry;
    };
}

#endif
