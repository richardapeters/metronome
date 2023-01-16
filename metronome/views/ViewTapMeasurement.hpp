#ifndef METRONOME_VIEW_TAP_MEASUREMENT_HPP
#define METRONOME_VIEW_TAP_MEASUREMENT_HPP

#include "infra/timer/TimerService.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "metronome/views/ViewBpm.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/views/ViewAligned.hpp"
#include "preview/views/ViewIcon.hpp"
#include "preview/views/ViewPanel.hpp"

namespace application
{
    class ViewTapMeasurement
        : public services::ViewPanel::WithView<services::ViewAligned::WithView<services::ViewIcon>>
        , public services::TouchRecipient
    {
    public:
        ViewTapMeasurement(BpmSelectionObserver& bpmSelectionObserver, infra::Colour inactiveColour, infra::Colour activeColour);

        // Implementation of TouchRecipient
        virtual void StartTouch(infra::Point point) override;
        virtual void DragIn(infra::Point point) override;
        virtual void DragTo(infra::Point point) override;
        virtual void DragOut() override;
        virtual void StopTouch() override;
        virtual void Swipe(services::Direction direction) override;
        virtual services::View& GetView() override;

    private:
        uint32_t ComputeAverage() const;

    private:
        BpmSelectionObserver& bpmSelectionObserver;
        infra::Colour inactiveColour;
        infra::Colour activeColour;
        infra::BoundedDeque<infra::TimePoint>::WithMaxSize<16> taps;
    };
}

#endif
