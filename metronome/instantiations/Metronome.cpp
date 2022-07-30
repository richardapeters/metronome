#include "metronome/instantiations/Metronome.hpp"

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

    NoteKindInteractor::NoteKindInteractor(ViewNoteKind& subject, BeatController& beatController)
        : ViewNoteKindObserver(subject)
        , beatController(beatController)
    {
        beatController.SelectedNoteKind(Subject().Value());
    }

    void NoteKindInteractor::SelectedNoteKind(uint8_t noteKind)
    {
        beatController.SelectedNoteKind(noteKind);
    }
}

namespace main_
{
    Metronome::Metronome(infra::Vector size, services::SettableTimerService& localTime, application::BeatTimer& beatTimer,
        hal::MultiBufferDisplay& display, hal::BitmapPainter& bitmapPainter, hal::SerialCommunication& serialMidi, infra::Bitmap& bitmap0, infra::Bitmap& bitmap1, infra::Bitmap& beatBitmap)
        : beatController(beatTimer)
        , notes(serialMidi, beatTimer)
        , beatPainter(display, beatTimer, bitmapPainter, bitmap0, bitmap1, beatBitmap)
        , viewPainter(beatPainter, bitmapPainter)
        , repainter(viewPainter, touch.GetView())
        , viewBpm(beatController, notes, bitmapPainter)
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
