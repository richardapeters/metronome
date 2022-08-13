#include "infra/event/EventDispatcher.hpp"
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

    ViewTimeline::ViewTimeline(Notes& notes, hal::BitmapPainter& painter, BeatTimer& beatTimer)
        : NotesObserver(notes)
        , BeatTimerObserver(beatTimer)
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

        {
            services::BitmapCanvas canvas(nowIndicator, painter);
            auto region = infra::Region(infra::Point(), canvas.Size());
            canvas.DrawFilledRectangle(region, infra::Colour::white, region);
            canvas.DrawFilledCircle(region.Centre(), 2, infra::Colour::blue, region);
        }
    }

    void ViewTimeline::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        for (const auto& line : lines)
            canvas.DrawLine(line.first, line.second, infra::Colour::darkGray, boundingRegion);

        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 - 2, ViewRegion().Size().deltaX / 3 + 2, infra::Colour::red, boundingRegion);

        for (const auto& note : notes)
            canvas.DrawTransparentBitmap(std::get<0>(note), *std::get<1>(note), infra::ConvertRgb888To(infra::Colour::white, std::get<1>(note)->pixelFormat), boundingRegion);

        canvas.DrawTransparentBitmap(nowIndicatorPosition, nowIndicator, infra::ConvertRgb888To(infra::Colour::white, nowIndicator.pixelFormat), boundingRegion);

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
            auto offsetFromCentre = ViewRegion().Size().deltaX / 3 - 5;
            auto arc = ConvertArc(position, lines.max_size());

            auto lineStart = infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre);
            auto lineEnd = infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre + 20 - 8 * (position % 2));

            lines.push_back({ lineStart, lineEnd });
        }
    }

    void ViewTimeline::NoteAdded(Note newNote)
    {
        if (notes.full())
            notes.pop_front();

        auto arc = ConvertArc(newNote.moment, std::numeric_limits<uint16_t>::max() + 1);
        auto [distance, bitmap] = PitchToDistanceAndBitmap(newNote.pitch);
        auto offsetFromCentre = ViewRegion().Size().deltaX / 3 - 4 + distance;

        auto origin = infra::RotatedPoint(ViewRegion().Centre(), arc, offsetFromCentre) - infra::Vector(2, 1);
        notes.emplace_back(origin, bitmap, newNote.moment);

#ifdef SHOW_PITCH
        if (newNotes.empty())
            lastPitch = infra::none;
        else
            lastPitch = newNotes.back().pitch;
#endif

        Dirty(infra::Region(origin, bitmap->size));
    }

    void ViewTimeline::Beat(uint8_t subDivision)
    {
        if (subDivision % 3 == 0)
            infra::EventDispatcher::Instance().Schedule([this, subDivision]()
                {
                    infra::Region dirtyRegion;

                    auto period = (std::numeric_limits<uint16_t>::max() + 1) / 4 / 4;
                    auto start = period * (beatIndex * 4 + subDivision / 3);
                    auto end = start + period;

                    auto i = notes.begin();
                    for (; i != notes.end(); ++i)
                    {
                        if (std::get<2>(*i) < start || std::get<2>(*i) >= end)
                            break;
 
                        dirtyRegion = dirtyRegion | infra::Region(std::get<0>(*i), std::get<1>(*i)->size);
                    }

                    notes.erase(notes.begin(), i);

                    dirtyRegion = dirtyRegion | infra::Region(nowIndicatorPosition, nowIndicator.size);
                    auto arc = ConvertArc(beatIndex * 4 + subDivision / 3, 16);
                    nowIndicatorPosition = infra::RotatedPoint(ViewRegion().Centre(), arc, ViewRegion().Size().deltaX / 3) - infra::Vector(2, 1);
                    dirtyRegion = dirtyRegion | infra::Region(nowIndicatorPosition, nowIndicator.size);

                    Dirty(dirtyRegion);

                    if (subDivision == 9)
                        beatIndex = (beatIndex + 1) % 4;
                });
    }

    float ViewTimeline::ConvertArc(uint32_t position, uint32_t scale) const
    {
        return 2 * pi / scale * position - 2 * pi / 4;
    }

    void ViewTimeline::Started(uint16_t bpm, infra::Optional<uint8_t> newBeatsPerMeasure)
    {
        notes.clear();
        beatIndex = 0;

        auto arc = ConvertArc(0, 16);
        nowIndicatorPosition = infra::RotatedPoint(ViewRegion().Centre(), arc, ViewRegion().Size().deltaX / 3) - infra::Vector(2, 1);

        Dirty(ViewRegion());
    }

    void ViewTimeline::Stopped()
    {}

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
