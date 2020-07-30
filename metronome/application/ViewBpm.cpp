#include "infra/stream/StringOutputStream.hpp"
#include "metronome/application/ViewBpm.hpp"
#include "preview/fonts/Fonts.hpp"
#include <cmath>

namespace application
{
    namespace
    {
        const auto pi = std::acos(-1);
    }

    const std::array<uint16_t, 8> ViewBpm::sprocketValues = { { 80, 90, 100, 120, 140, 160, 60, 70 } };

    ViewBpm::ViewBpm(BeatController& controller)
        : BeatControllerObserver(controller)
    {}

    void ViewBpm::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        canvas.DrawFilledRectangle(ViewRegion(), backgroundColour, boundingRegion);

        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 + 2, ViewRegion().Size().deltaX / 3 + 3, BetweenColour(infra::Colour::red, infra::Colour::white, 192), boundingRegion);
        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 - 3, ViewRegion().Size().deltaX / 3 - 2, BetweenColour(infra::Colour::red, infra::Colour::white, 192), boundingRegion);
        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 + 1, ViewRegion().Size().deltaX / 3 + 2, BetweenColour(infra::Colour::red, infra::Colour::white, 64), boundingRegion);
        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 - 2, ViewRegion().Size().deltaX / 3 - 1, BetweenColour(infra::Colour::red, infra::Colour::white, 64), boundingRegion);
        canvas.DrawCircle(ViewRegion().Centre(), ViewRegion().Size().deltaX / 3 - 1, ViewRegion().Size().deltaX / 3 + 1, infra::Colour::red, boundingRegion);

        auto& font = infra::freeSans24pt7b;
        canvas.DrawString(ViewRegion().Centre() + infra::Vector(-font.Width(bpm) / 2, font.cursorToTop / 2), bpm, font, infra::Colour::blue, boundingRegion);

        if (mode == TouchMode::wheel)
            for (int sprocket = 0; sprocket != 8; ++sprocket)
                DrawSprocket(canvas, boundingRegion, sprocket);
    }

    void ViewBpm::StartTouch(infra::Point point)
    {
        if (infra::Distance(ViewRegion().Centre(), point) < static_cast<uint16_t>(ViewRegion().Size().deltaX / 6))
            mode = TouchMode::wheel;
        else
        {
            int16_t distanceToTop = point.y;
            int16_t distanceToBottom = ViewRegion().Size().deltaY - point.y;
            int16_t distanceToLeft = point.x;
            int16_t distanceToRight = ViewRegion().Size().deltaX - point.x;

            if (std::min(distanceToLeft, distanceToRight) < std::min(distanceToTop, distanceToBottom))
                mode = TouchMode::bpmMulti;
            else
                mode = TouchMode::bpmSingle;

            stepsReported = 0;
        }

        startTouch = point;
        touchRemainedAtStart = true;

        Dirty(ViewRegion());
    }

    void ViewBpm::DragIn(infra::Point point)
    {
        StartTouch(point);
    }

    void ViewBpm::DragTo(infra::Point point)
    {
        if (infra::Distance(point, *startTouch) > static_cast<uint16_t>(ViewRegion().Size().deltaY / 16))
            touchRemainedAtStart = false;

        switch (mode)
        {
            case TouchMode::wheel:
            {
                if (infra::Distance(point, *startTouch) > static_cast<uint16_t>(ViewRegion().Size().deltaY / 6))
                {
                    selectedSprocket = 0;
                    auto selectedSprocketDistance = infra::Distance(point, SprocketCentre(*selectedSprocket));
                    for (int sprocket = 1; sprocket != 8; ++sprocket)
                        if (infra::Distance(point, SprocketCentre(sprocket)) < selectedSprocketDistance)
                        {
                            selectedSprocket = sprocket;
                            selectedSprocketDistance = infra::Distance(point, SprocketCentre(*selectedSprocket));
                        }
                }
                else
                    selectedSprocket = infra::none;
                break;
            }
            case TouchMode::bpmMulti:
            {
                auto steps = (point.x - startTouch->x) / (ViewRegion().Size().deltaX / 4) - stepsReported;
                stepsReported += steps;
                NotifyObservers([steps](BpmSelectionObserver& observer) { observer.MultiStep(steps); });
                break;
            }
            case TouchMode::bpmSingle:
            {
                auto steps = (startTouch->y - point.y) / (ViewRegion().Size().deltaY / 20) - stepsReported;
                stepsReported += steps;
                NotifyObservers([steps](BpmSelectionObserver& observer) { observer.SingleStep(steps); });
                break;
            }
        }

        Dirty(ViewRegion());
    }

    void ViewBpm::DragOut()
    {
        StopTouch();
    }

    void ViewBpm::StopTouch()
    {
        if (mode == TouchMode::wheel && selectedSprocket != infra::none)
            NotifyObservers([this](BpmSelectionObserver& observer) { observer.BpmSelected(sprocketValues[*selectedSprocket]); });

        if (touchRemainedAtStart)
            NotifyObservers([](BpmSelectionObserver& observer) { observer.ToggleStart(); });

        mode = TouchMode::idle;
        startTouch = infra::none;

        Dirty(ViewRegion());
    }

    void ViewBpm::Swipe(services::Direction direction)
    {}

    services::View& ViewBpm::GetView()
    {
        return *this;
    }

    void ViewBpm::SetBpm(uint16_t newBpm)
    {
        bpm.clear();
        infra::StringOutputStream stream(bpm);
        stream << newBpm;

        Dirty(ViewRegion());
    }

    void ViewBpm::PrepareBeat()
    {
        proportion = 0;
        rollOff.Start(std::chrono::milliseconds(35), [this]()
        {
            backgroundColour = BetweenColour(infra::Colour::red, infra::Colour::white, proportion);
            if (proportion == 256)
                rollOff.Cancel();
            else
                proportion += 64;

            Dirty(ViewRegion());
        }, infra::triggerImmediately);
    }

    void ViewBpm::DrawSprocket(hal::Canvas& canvas, infra::Region boundingRegion, int sprocket) const
    {
        infra::StringOutputStream::WithStorage<4> value;
        value << sprocketValues[sprocket];

        auto point = SprocketCentre(sprocket);
        auto& font = infra::freeSans9pt7b;
        canvas.DrawFilledCircle(point, ViewRegion().Size().deltaX / 16, selectedSprocket == sprocket ? infra::Colour::red : infra::Colour::blue, boundingRegion);
        canvas.DrawString(point + infra::Vector(-font.Width(value.Storage()) / 2, font.cursorToTop / 2), value.Storage(), font, infra::Colour::white, boundingRegion);
    }

    infra::Point ViewBpm::SprocketCentre(int sprocket) const
    {
        auto offsetFromCentre = ViewRegion().Size().deltaX / 3;
        return ViewRegion().Centre()
            + infra::Vector(static_cast<int16_t>(std::cos(2 * pi * sprocket / 8) * offsetFromCentre), static_cast<int16_t>(std::sin(2 * pi * sprocket / 8) * offsetFromCentre));
    }
}
