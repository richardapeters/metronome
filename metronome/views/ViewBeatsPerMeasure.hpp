#ifndef METRONOME_VIEW_BEATS_PER_MEASURE_HPP
#define METRONOME_VIEW_BEATS_PER_MEASURE_HPP

#include "infra/util/Observer.hpp"
#include "preview/touch/TouchSpinInteger.hpp"
#include "preview/views/ViewDisableableText.hpp"

namespace application
{
    class ViewBeatsPerMeasure;

    class ViewBeatsPerMeasureObserver
        : public infra::Observer<ViewBeatsPerMeasureObserver, ViewBeatsPerMeasure>
    {
    public:
        using infra::Observer<ViewBeatsPerMeasureObserver, ViewBeatsPerMeasure>::Observer;

        virtual void SelectedBeatsPerMeasure(uint8_t beatsPerMeasure) = 0;
        virtual void DisabledBeatsPerMinute() = 0;
    };

    class ViewBeatsPerMeasure
        : public services::TouchSpinInteger::WithViewFadingTextDescendant<services::ViewDisableableFadingText>::WithStorage<2>
        , public infra::Subject<ViewBeatsPerMeasureObserver>
    {
    public:
        ViewBeatsPerMeasure();

        virtual void StartTouch(infra::Point point) override;
        virtual void DragIn(infra::Point point) override;
        virtual void StopTouch() override;

    protected:
        virtual void Report(int32_t steps, services::Direction from) override;

    private:
        bool enabled = true;
        bool touchRemainedAtStart;

        using Parent = services::TouchSpinInteger::WithViewFadingTextDescendant<services::ViewDisableableFadingText>::WithStorage<2>;
    };
}

#endif
