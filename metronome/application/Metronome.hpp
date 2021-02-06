#ifndef METRONOME_METRONOME_HPP
#define METRONOME_METRONOME_HPP

#include "metronome/application/BpmSelectionInteractor.hpp"
#include "metronome/application/Ds3231.hpp"
#include "metronome/application/ViewBeatsPerMeasure.hpp"
#include "metronome/application/ViewBpm.hpp"
#include "metronome/application/ViewDateEntry.hpp"
#include "metronome/application/ViewNoteKind.hpp"
#include "metronome/application/ViewPainterMetronome.hpp"
#include "metronome/application/ViewTapMeasurement.hpp"
#include "preview/interfaces/DoubleBufferDisplay.hpp"
#include "preview/interfaces/BitmapPainter.hpp"
#include "preview/touch/TouchButton.hpp"
#include "preview/touch/TouchHorizontalLayout.hpp"
#include "preview/touch/TouchPanel.hpp"
#include "preview/touch/TouchVerticalLayout.hpp"
#include "preview/touch/TouchViewMultiple.hpp"
#include "preview/views/ViewButtonPanel.hpp"
#include "preview/views/ViewCurrentTime.hpp"

namespace main_
{
    struct Metronome
    {
        Metronome(infra::Vector size, services::SettableTimerService& localTime, application::BeatTimer& beatTimer, hal::DoubleBufferDisplay& display, hal::BitmapPainter& bitmapPainter);

        void StartTimeEntry();
        void StopTimeEntry(infra::TimePoint newTime);

        application::BeatControllerImpl beatController;
        services::TouchViewMultiple::WithMaxViews<2> touch;
        application::ViewPainterMetronome viewPainter;
        services::TouchHorizontalLayout::WithMaxViews<2> touchBpm;
        application::ViewBpm viewBpm{ beatController };
        application::BpmSelectionInteractor bpmSelectionObserver{ viewBpm, beatController };
        services::TouchVerticalLayout::WithMaxViews<3> touchVertical;
        application::ViewTapMeasurement viewTapMeasurement{ bpmSelectionObserver, infra::Colour::lightGray, infra::Colour::darkGray };
        services::TouchHorizontalLayout::WithMaxViews<2> touchHorizontalRight;
        services::TouchPanel::WithView<application::ViewBeatsPerMeasure> viewBeatsPerMeasure{ infra::Colour::lightGray };
        services::TouchPanel::WithView<application::ViewNoteKind> viewNoteKind{ infra::Colour::lightGray };
        services::TouchButton::With<services::ViewButtonPanel::WithView<services::ViewCurrentTime>> viewCurrentTime;

        services::SettableTimerService& localTime;
        infra::Optional<services::TouchAligned::WithView<application::ViewDateEntry>> dateEntry;
    };
}

#endif
