#include "infra/stream/StringOutputStream.hpp"
#include "metronome/views/ViewBpm.hpp"
#include "preview/fonts/Fonts.hpp"

namespace application
{
    const std::array<uint16_t, 8> ViewBpm::selectValues = { { 80, 90, 100, 120, 140, 160, 60, 70 } };

    ViewBpm::ViewBpm(BeatController& controller, Notes& notes, hal::BitmapPainter& painter, BeatTimer& beatTimer)
        : controller(controller)
        , valueSelect(selectValues, [this](uint16_t value) { Select(value); }, 0)
        , valueFastUp(fastUpValues, [this](uint16_t value) { Select(value); }, 0)
        , valueFastDown(fastDownValues, [this](uint16_t value) { Select(value); }, 0)
        , valueSlowUp(slowUpValues, [this](uint16_t value) { Select(value); }, 1)
        , valueSlowDown(slowDownValues, [this](uint16_t value) { Select(value); }, 3)
        , timeline(notes, painter, beatTimer)
    {
        valueSelect.SetParent(*this);
        valueFastUp.SetParent(*this);
        valueFastDown.SetParent(*this);
        valueSlowUp.SetParent(*this);
        valueSlowDown.SetParent(*this);
        timeline.SetParent(*this);
    }

    void ViewBpm::Paint(hal::Canvas& canvas, infra::Region boundingRegion)
    {
        canvas.DrawFilledRectangle(ViewRegion(), infra::Colour::white, boundingRegion);

        auto& font = infra::freeSans24pt7b;
        canvas.DrawString(ViewRegion().Centre() + infra::Vector(-font.Width(bpmString) / 2, font.cursorToTop / 2), bpmString, font, infra::Colour::blue, infra::RightAngle::angle_0, boundingRegion);

        timeline.Paint(canvas, boundingRegion);
        if (currentSprocket != nullptr)
            currentSprocket->Paint(canvas, boundingRegion);
    }

    void ViewBpm::ViewRegionChanged()
    {
        valueSelect.SetViewRegion(ViewRegion());
        valueFastUp.SetViewRegion(ViewRegion());
        valueFastDown.SetViewRegion(ViewRegion());
        valueSlowUp.SetViewRegion(ViewRegion());
        valueSlowDown.SetViewRegion(ViewRegion());
        timeline.SetViewRegion(ViewRegion());
    }

    void ViewBpm::StartTouch(infra::Point point)
    {
        if (infra::Distance(ViewRegion().Centre(), point) < static_cast<uint16_t>(ViewRegion().Size().deltaX / 6))
        {
            currentSprocket = &valueSelect;
            valueSelect.StartTouch(point);
        }
        else
        {
            int16_t distanceToTop = point.y;
            int16_t distanceToBottom = ViewRegion().Size().deltaY - point.y;
            int16_t distanceToLeft = point.x;
            int16_t distanceToRight = ViewRegion().Size().deltaX - point.x;

            if (std::min(distanceToLeft, distanceToRight) < std::min(distanceToTop, distanceToBottom))
            {
                if (distanceToLeft < distanceToRight)
                {
                    currentSprocket = &valueFastUp;

                    fastUpValues[0] = bpm + 20;
                    fastUpValues[3] = bpm + 10;
                    fastUpValues[1] = bpm + 5;

                    for (int i = 0; i != fastUpValues.size(); ++i)
                        if (!controller.BpmIsValid(fastUpValues[i]))
                            fastUpValues[i] = 0;
                }
                else
                {
                    currentSprocket = &valueFastDown;

                    fastDownValues[2] = bpm - 20;
                    fastDownValues[3] = bpm - 10;
                    fastDownValues[1] = bpm - 5;

                    for (int i = 0; i != fastDownValues.size(); ++i)
                        if (!controller.BpmIsValid(fastDownValues[i]))
                            fastDownValues[i] = 0;
                }
            }
            else
            {
                if (distanceToTop < distanceToBottom)
                {
                    currentSprocket = &valueSlowDown;

                    slowDownValues[0] = 0;
                    for (int i = 1; i != slowDownValues.size(); ++i)
                    {
                        slowDownValues[i] = bpm - i;
                        if (!controller.BpmIsValid(slowDownValues[i]))
                            slowDownValues[i] = 0;
                    }
                }
                else
                {
                    currentSprocket = &valueSlowUp;

                    slowUpValues[0] = 0;
                    for (int i = 1; i != slowUpValues.size(); ++i)
                    {
                        slowUpValues[i] = bpm + i;
                        if (!controller.BpmIsValid(slowUpValues[i]))
                            slowUpValues[i] = 0;
                    }
                }
            }

            if (currentSprocket != nullptr)
                currentSprocket->StartTouch(point);
        }

        startTouch = point;

        Dirty(ViewRegion());
    }

    void ViewBpm::DragIn(infra::Point point)
    {
        StartTouch(point);
    }

    void ViewBpm::DragTo(infra::Point point)
    {
        if (currentSprocket != nullptr)
            currentSprocket->DragTo(point);
    }

    void ViewBpm::DragOut()
    {
        StopTouch();
    }

    void ViewBpm::StopTouch()
    {
        if (currentSprocket != nullptr)
            currentSprocket->StopTouch();
        currentSprocket = nullptr;

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
        bpmString.clear();
        infra::StringOutputStream stream(bpmString);
        stream << newBpm;
        bpm = newBpm;

        Dirty(ViewRegion());
    }

    void ViewBpm::Select(uint16_t value)
    {
        NotifyObservers([value](BpmSelectionObserver& observer) { observer.BpmSelected(value); });
    }
}
