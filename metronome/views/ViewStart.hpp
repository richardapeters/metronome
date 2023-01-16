#ifndef METRONOME_VIEW_START_HPP
#define METRONOME_VIEW_START_HPP

#include "metronome/interfaces/BeatController.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/views/ViewAligned.hpp"
#include "preview/views/ViewIcon.hpp"
#include "preview/views/ViewPanel.hpp"

namespace application
{
    class ViewStart
        : public services::ViewPanel::WithView<services::ViewAligned::WithView<services::ViewIcon>>
        , public services::TouchRecipient
    {
    public:
        ViewStart(BeatController& beatController, infra::Colour backgroundColour);

        // Implementation of TouchRecipient
        virtual void StartTouch(infra::Point point) override;
        virtual void DragIn(infra::Point point) override;
        virtual void DragTo(infra::Point point) override;
        virtual void DragOut() override;
        virtual void StopTouch() override;
        virtual void Swipe(services::Direction direction) override;
        virtual services::View& GetView() override;

    private:
        BeatController& beatController;
    };
}

#endif
