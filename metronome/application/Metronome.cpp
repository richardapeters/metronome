#include "infra/stream/StringOutputStream.hpp"
#include "metronome/application/Metronome.hpp"
#include "metronome/application/NoteFont.hpp"
#include "preview/fonts/Fonts.hpp"

namespace main_
{
    Metronome::Metronome(infra::Vector size, services::SettableTimerService& localTime, application::BeatTimer& beatTimer, hal::DoubleBufferDisplay& display, hal::BitmapPainter& bitmapPainter)
        : beatController(beatTimer)
        , viewPainter(display, bitmapPainter, touch.GetView(), beatController)
        , viewCurrentTime([this]() { StartTimeEntry(); }, infra::Colour::lightGray, infra::Colour::darkGray, localTime.Id(), services::TextAttributes{ infra::Colour::blue, infra::freeSans12pt7b })
        , localTime(localTime)
    {
        touch.Add(touchBpm);
        touchBpm.AddFill(viewBpm, size.deltaY);
        touchBpm.AddFill(touchVertical, size.deltaX - size.deltaY);
        touchVertical.AddFill(viewTapMeasurement, 1);
        touchVertical.AddFill(touchHorizontalRight, 3);
        touchHorizontalRight.AddFill(viewBeatsPerMeasure, 1);
        touchHorizontalRight.AddFill(viewNoteKind, 1);
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
