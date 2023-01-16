#ifndef METRONOME_VIEW_SPROCKET_HPP
#define METRONOME_VIEW_SPROCKET_HPP

#include "infra/util/Function.hpp"
#include "preview/interfaces/View.hpp"
#include "preview/touch/TouchRecipient.hpp"

namespace application
{
    class ViewSprocket
        : public services::View
        , public services::TouchRecipient
    {
    public:
        ViewSprocket(infra::MemoryRange<const uint16_t> values, const infra::Function<void(uint16_t)>& onSelected, int adjustStart);

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;

        // Implementation of TouchRecipient
        virtual void StartTouch(infra::Point point) override;
        virtual void DragIn(infra::Point point) override;
        virtual void DragTo(infra::Point point) override;
        virtual void DragOut() override;
        virtual void StopTouch() override;
        virtual void Swipe(services::Direction direction) override;
        virtual services::View& GetView() override;

    private:
        void DrawTooth(hal::Canvas& canvas, infra::Region boundingRegion, int tooth) const;
        infra::Point ToothCentre(int tooth) const;

    private:
        infra::MemoryRange<const uint16_t> values;
        infra::Function<void(uint16_t)> onSelected;
        int adjustStart;

        infra::Optional<infra::Point> startTouch;
        infra::Optional<int> selectedTooth;
    };
}

#endif
