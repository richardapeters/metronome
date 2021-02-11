#include "infra/stream/StringOutputStream.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/NoteFont.hpp"
#include "preview/fonts/Fonts.hpp"

namespace application
{
    BeatsPerMeasureInteractor::BeatsPerMeasureInteractor(ViewBeatsPerMeasure& subject, BeatController& beatController)
        : ViewBeatsPerMeasureObserver(subject)
        , beatController(beatController)
    {
        beatController.SelectedBeatsPerMeasure(Subject().Value());
    }

    void BeatsPerMeasureInteractor::SelectedBeatsPerMeasure(uint8_t beatsPerMeasure)
    {
        beatController.SelectedBeatsPerMeasure(beatsPerMeasure);
    }

    void BeatsPerMeasureInteractor::DisabledBeatsPerMinute()
    {
        beatController.DisabledBeatsPerMinute();
    }
}

namespace main_
{
    Metronome::Metronome(infra::Vector size, services::SettableTimerService& localTime, application::MetronomeBeatTimer& beatTimer, hal::DoubleBufferDisplay& display, hal::BitmapPainter& bitmapPainter)
        : beatController(beatTimer)
        , viewPainter(display, bitmapPainter, touch.GetView(), beatController)
        , viewCurrentTime([this]() { StartTimeEntry(); }, infra::Colour::lightGray, infra::Colour::darkGray, localTime.Id(), services::TextAttributes{ infra::Colour::blue, infra::freeSans12pt7b })
        , localTime(localTime)
    {
        touch.Add(touchBpm);
        touchBpm.AddFill(viewBpm, size.deltaY);
        touchBpm.AddFill(touchVertical, size.deltaX - size.deltaY);
        touchVertical.AddFill(touchHorizontalTop, 1);
        touchHorizontalTop.AddFill(viewStart, 1);
        touchHorizontalTop.AddFill(viewTapMeasurement, 1);
        touchVertical.AddFill(touchHorizontalMid, 3);
        touchHorizontalMid.AddFill(viewBeatsPerMeasure, 1);
        touchHorizontalMid.AddFill(viewNoteKind, 1);
        touchVertical.AddFill(viewCurrentTime, 1);
        touch.GetView().ResetLayout(size);
    }

    void Metronome::StartTimeEntry()
    {
        dateEntry.Emplace(localTime.Now(), [this](infra::TimePoint newTime) { StopTimeEntry(newTime); });
        touch.Add(*dateEntry);
    }

    void Metronome::StopTimeEntry(infra::TimePoint newTime)
    {
        touch.Remove(*dateEntry);
        dateEntry = infra::none;
        localTime.SetTime(newTime, [this]() {});
    }
}
