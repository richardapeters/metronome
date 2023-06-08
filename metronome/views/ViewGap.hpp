#ifndef METRONOME_VIEW_GAP_HPP
#define METRONOME_VIEW_GAP_HPP

#include "infra/util/Observer.hpp"
#include "preview/touch/TouchSpinInteger.hpp"

namespace application
{
    class ViewGap;

    class ViewGapObserver
        : public infra::Observer<ViewGapObserver, ViewGap>
    {
    public:
        using infra::Observer<ViewGapObserver, ViewGap>::Observer;

        virtual void SelectedGap(uint8_t gap) = 0;
    };

    class ViewGap
        : public services::TouchSpinInteger::WithViewFadingText::WithStorage<2>
        , public infra::Subject<ViewGapObserver>
    {
    public:
        ViewGap();

    protected:
        virtual void Report(int32_t steps, services::Direction from) override;

    private:
        using Parent = services::TouchSpinInteger::WithViewFadingText::WithStorage<2>;
    };
}

#endif
