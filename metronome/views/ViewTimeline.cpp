#include "infra/stream/StringOutputStream.hpp"
#include "metronome/views/ViewTimeline.hpp"
#include "preview/fonts/Fonts.hpp"
#include "preview/interfaces/BitmapCanvas.hpp"
#include <cmath>

namespace application
{
    namespace
    {
        const auto pi = std::acos(-1);
    }

    ViewTimeline::ViewTimeline(Notes& notes, hal::BitmapPainter& painter)
        : NotesObserver(notes)
    {
        {
            services::BitmapCanvas canvas(noteTom, painter);
            auto region = infra::Region(infra::Point(), canvas.Size());
            canvas.DrawFilledRectangle(region, infra::Colour::white, region);
            canvas.DrawFilledCircle(region.Centre(), 2, infra::Colour::black, region);
        }

        {
            services::BitmapCanvas canvas(noteHiHat, painter);
            auto region = infra::Region(infra::Point(), canvas.Size());
            canvas.DrawFilledRectangle(region, infra::Colour::white, region);
            canvas.DrawLine(infra::Point(0, 0), infra::Point(4, 4), infra::Colour::black, region);
            canvas.DrawLine(infra::Point(0, 4), infra::Point(4, 0), infra::Colour::black, region);
        }

        {
            services::BitmapCanvas canvas(noteCymbal, painter);
            auto region = infra::Region(infra::Point(), canvas.Size());
            canvas.DrawFilledRectangle(region, infra::Colour::white, region);
            canvas.DrawLine(infra::Point(0, 0), infra::Point(4, 4), infra::Colour::black, region);
            canvas.DrawLine(infra::Point(0, 4), infra::Point(4, 0), infra::Colour::black, region);
            canvas.DrawLine(infra::Point(0, 2), infra::Point(4, 2), infra::Colour::black, region);
        }
    }

    void ViewTimeline::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        for (const auto& line : lines)
            canvas.DrawLine(line.first, line.second, infra::Colour::darkGray, boundingRegion);

        for (const auto& note : notes)
            canvas.DrawBitmap(note.first, *note.second, boundingRegion);

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
            auto [distance, bitmap] = PitchToDistanceAndBitmap(note.pitch);
            auto offsetFromCentre = ViewRegion().Size().deltaX / 3 - 4 + distance;

            notes.emplace_back(infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre) - infra::Vector(2, 1), bitmap);
        }

#ifdef SHOW_PITCH
        if (newNotes.empty())
            lastPitch = infra::none;
        else
            lastPitch = newNotes.back().pitch;
#endif

        Dirty(ViewRegion());
    }

    std::pair<uint8_t, const infra::Bitmap*> ViewTimeline::PitchToDistanceAndBitmap(uint8_t pitch) const
    {
        switch (pitch)
        {
            case 36:        // Base
                return std::make_pair(0, &noteTom);
            case 43:        // Tom 3
                return std::make_pair(14, &noteTom);
            case 45:        // Tom 2
                return std::make_pair(12, &noteTom);
            case 48:        // Tom 1
                return std::make_pair(10, &noteTom);
            case 38:        // Snare
            case 40:        //       rim
                return std::make_pair(8, &noteTom);
            case 44:        // Hi-hat
            case 46:        // Hi-hat bow
            case 26:        //        edge
                return std::make_pair(12, &noteHiHat);
            case 51:        // Ride edge
            case 59:        //      bow
            case 53:        //      bell
                return std::make_pair(14, &noteHiHat);
            case 49:        // Crash 1 bow
            case 55:        //         edge
                return std::make_pair(16, &noteCymbal);
            case 57:        // Crash 2 bow
            case 52:        //         edge
                return std::make_pair(18, &noteCymbal);
            default:
                return std::make_pair(0, &noteCymbal);
        }
    }
}
