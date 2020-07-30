#include "metronome/application/ViewBeatsPerMeasure.hpp"
#include "preview/fonts/Fonts.hpp"

namespace application
{
    ViewBeatsPerMeasure::ViewBeatsPerMeasure()
        : services::TouchSpinInteger::WithViewFadingTextDescendant<services::ViewDisableableFadingText>::WithStorage<2>
            (4, 2, 16, false, 45, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::lightGray, infra::freeSans24pt7b })
    {}

    void ViewBeatsPerMeasure::StartTouch(infra::Point point)
    {
        touchRemainedAtStart = true;
        Parent::StartTouch(point);
    }

    void ViewBeatsPerMeasure::DragIn(infra::Point point)
    {
        Parent::DragIn(point);
        touchRemainedAtStart = false;
    }

    void ViewBeatsPerMeasure::StopTouch()
    {
        Parent::StopTouch();

        if (touchRemainedAtStart)
        {
            enabled = !enabled;
            GetView().Enable(enabled);
        }
    }

    void ViewBeatsPerMeasure::Report(int32_t steps, services::Direction from)
    {
        Parent::Report(steps, from);

        if (steps != 0)
            touchRemainedAtStart = false;
    }
}
