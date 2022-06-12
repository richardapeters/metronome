#include "infra/stream/StringOutputStream.hpp"
#include "metronome/views/ViewTimeline.hpp"
#include "preview/fonts/Fonts.hpp"
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
            canvas.DrawFilledRectangle(note, infra::Colour::black, boundingRegion);

#ifdef SHOW_PITCH
        if (lastPitch)
        {
            infra::StringOutputStream::WithStorage<5> pitch;
            pitch << *lastPitch;
            canvas.DrawString(ViewRegion().Centre(), pitch.Storage(), infra::freeSans24pt7b, infra::Colour::red, infra::RightAngle::angle_0, boundingRegion);
        }
#endif
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
            auto offsetFromCentre = ViewRegion().Size().deltaX / 3 + 5 + PitchToDistance(note.pitch);


            notes.push_back(infra::Region(infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre) - infra::Vector(1, 1), infra::Vector(3, 3)));
        }

#ifdef SHOW_PITCH
        if (newNotes.empty())
            lastPitch = infra::none;
        else
            lastPitch = newNotes.back().pitch;
#endif

        Dirty(ViewRegion());
    }

    uint8_t ViewTimeline::PitchToDistance(uint8_t pitch) const
    {
        switch (pitch)
        {
            default:
                return 0;
        }
    }
}
