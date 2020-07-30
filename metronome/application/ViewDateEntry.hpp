#ifndef METRONOME_VIEW_DATE_ENTRY_HPP
#define METRONOME_VIEW_DATE_ENTRY_HPP

#include "preview/fonts/Fonts.hpp"
#include "preview/touch/TouchAligned.hpp"
#include "preview/touch/TouchHorizontalLayout.hpp"
#include "preview/touch/TouchVerticalLayout.hpp"
#include "preview/touch/TouchButton.hpp"
#include "preview/touch/TouchPanel.hpp"
#include "preview/touch/TouchSpinInteger.hpp"
#include "preview/touch/TouchTitledFrame.hpp"
#include "preview/views/ViewFramedTextButton.hpp"

namespace application
{
    class ViewDateEntry
        : public services::TouchTitledFrame::WithTouch<services::TouchPanel>
    {
    public:
        ViewDateEntry(infra::TimePoint currentTime, const infra::Function<void(infra::TimePoint)>& onSet);

    private:
        void OnOk();

    private:
        services::TouchVerticalLayout::WithMaxViews<2> layout{ 0, 20, 40, 20, 40 };
        services::TouchAligned::WithView<services::TouchHorizontalLayout::WithMaxViews<4>> timeLayout;
        services::TouchSpinInteger::WithViewFadingText::WithStorage<2> hours;
        services::ViewText colon{ { infra::Colour::blue, infra::freeSans24pt7b}, ":" };;
        services::TouchSpinInteger::WithViewFadingText::WithStorage<1> minutesTen;
        services::TouchSpinInteger::WithViewFadingText::WithStorage<1> minutesOne;
        services::TouchAligned::WithView<services::TouchButton::With<services::ViewFramedTextButton>> ok;
        infra::Function<void(infra::TimePoint)> onSet;

        uint8_t days;
        uint8_t months;
        uint16_t years;
    };
}

#endif
