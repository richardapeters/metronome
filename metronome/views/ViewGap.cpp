#include "metronome/views/ViewGap.hpp"
#include "preview/fonts/Fonts.hpp"

namespace application
{
    ViewGap::ViewGap()
        : services::TouchSpinInteger::WithViewFadingText::WithStorage<2>(0, 0, 4, false, 45, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::lightGray, infra::freeSans24pt7b })
    {}

    void ViewGap::Report(int32_t steps, services::Direction from)
    {
        Parent::Report(steps, from);

        NotifyObservers([this](ViewGapObserver& observer)
            { observer.SelectedGap(Value()); });
    }
}
