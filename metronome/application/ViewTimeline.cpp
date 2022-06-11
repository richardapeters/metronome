#include "metronome/application/ViewTimeline.hpp"
#include <cmath>

namespace application
{
    namespace
    {
        const auto pi = std::acos(-1);
    }

    void ViewTimeline::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        for (const auto& line : lines)
            canvas.DrawLine(line.first, line.second, infra::Colour::darkGray, boundingRegion);

        for (const auto& note : notes)
            canvas.DrawFilledRectangle(infra::Region(note, infra::Vector(3, 3)), infra::Colour::black, boundingRegion);
    }

    void ViewTimeline::ViewRegionChanged()
    {
        lines.clear();

        while (!lines.full())
        {
            auto position = lines.size();
            auto offsetFromCentre = ViewRegion().Size().deltaX / 3 + 5;
            auto arc = 2 * pi * position / lines.max_size() + 2 * pi / 4;

            lines.push_back({ infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre), infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre + 10 - 8 * (position % 2)) });
        }
    }

    void ViewTimeline::NotesChanged(infra::MemoryRange<const Note> newNotes)
    {
        notes.clear();

        for (auto note : newNotes)
        {
            if (notes.full())
                break;

            auto arc = 2 * pi / std::numeric_limits<uint16_t>::max() * note.moment - 2 * pi / 4;
            auto offsetFromCentre = ViewRegion().Size().deltaX / 3 + 5;

            notes.push_back(infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre));
        }

        Dirty(ViewRegion());
    }
}
