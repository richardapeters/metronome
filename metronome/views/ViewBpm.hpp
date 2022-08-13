#ifndef METRONOME_VIEW_BPM_HPP
#define METRONOME_VIEW_BPM_HPP

#include "metronome/interfaces/BeatController.hpp"
#include "metronome/interfaces/BpmSelection.hpp"
#include "metronome/views/ViewSprocket.hpp"
#include "metronome/views/ViewTimeline.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/interfaces/View.hpp"

namespace application
{
    class ViewBpm
        : public services::View
        , public services::TouchRecipient
        , public BpmSelection
    {
    public:
        ViewBpm(BeatController& controller, Notes& notes, hal::BitmapPainter& painter, BeatTimer& beatTimer);

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
        virtual void ViewRegionChanged() override;

        // Implementation of TouchRecipient
        virtual void StartTouch(infra::Point point) override;
        virtual void DragIn(infra::Point point) override;
        virtual void DragTo(infra::Point point) override;
        virtual void DragOut() override;
        virtual void StopTouch() override;
        virtual void Swipe(services::Direction direction) override;
        virtual services::View& GetView() override;

        // Implementation of BpmSelection
        virtual void SetBpm(uint16_t bpm) override;

    private:
        void Select(uint16_t value);

    private:
        BeatController& controller;

        infra::BoundedString::WithStorage<4> bpmString;
        uint16_t bpm;
        infra::Colour backgroundColour = infra::Colour::white;

        infra::Optional<infra::Point> startTouch;
        int stepsReported;

        ViewSprocket valueSelect;
        ViewSprocket valueFastUp;
        ViewSprocket valueFastDown;
        ViewSprocket valueSlowUp;
        ViewSprocket valueSlowDown;

        ViewSprocket* currentSprocket = nullptr;

        ViewTimeline timeline;

        static const std::array<uint16_t, 8> selectValues;
        std::array<uint16_t, 4> fastUpValues{};
        std::array<uint16_t, 4> fastDownValues{};
        std::array<uint16_t, 11> slowUpValues{};
        std::array<uint16_t, 11> slowDownValues{};
    };
}

#endif
