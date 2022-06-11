#include "infra/timer/PartitionedTime.hpp"
#include "metronome/views/ViewDateEntry.hpp"

namespace application
{
    ViewDateEntry::ViewDateEntry(infra::TimePoint currentTime, const infra::Function<void(infra::TimePoint)>& onSet)
        : services::TouchTitledFrame::WithTouch<services::TouchPanel>(services::TitledFrameAttributes{ infra::Colour::black, infra::Colour::gray, infra::Colour::blue, infra::freeSans12pt7b }, "Enter time", infra::Colour::gray)
        , hours(0, 0, 23, true, 30, 2, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::gray, infra::freeSans24pt7b })
        , minutesTen(0, 0, 5, true, 30, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::gray, infra::freeSans24pt7b })
        , minutesOne(0, 0, 9, true, 30, services::FadingTextAttributes{ infra::Colour::blue, infra::Colour::gray, infra::freeSans24pt7b })
        , ok([this]() { OnOk(); }, services::FramedTextButtonAttributes{ infra::Colour::darkGray, infra::Colour::gray, infra::Colour::darkGray, infra::Colour::white, infra::freeSans12pt7b }, "OK")
        , onSet(onSet)
    {
        infra::PartitionedTime partitioned(currentTime);
        hours.SetValue(partitioned.hours);
        minutesOne.SetValue(partitioned.minutes % 10);
        minutesTen.SetValue(partitioned.minutes / 10);
        days = partitioned.days;
        months = partitioned.months;
        years = partitioned.years;

        layout.AddFill(timeLayout);
        layout.AddFill(ok);

        timeLayout.SubView().Add(hours);
        timeLayout.SubView().GetView().AddFill(colon);
        timeLayout.SubView().Add(minutesTen);
        timeLayout.SubView().Add(minutesOne);

        SetSubView(layout);
    }

    void ViewDateEntry::OnOk()
    {
        onSet(infra::PartitionedTime(years, months, days, hours.Value(), minutesOne.Value() + 10 * minutesTen.Value(), 0).ToTimePoint());
    }
}
