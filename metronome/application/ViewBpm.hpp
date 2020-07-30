#ifndef METRONOME_VIEW_BPM_HPP
#define METRONOME_VIEW_BPM_HPP

#include "infra/util/Observer.hpp"
#include "infra/timer/Timer.hpp"
#include "metronome/application/BeatController.hpp"
#include "preview/touch/TouchRecipient.hpp"
#include "preview/interfaces/View.hpp"

namespace application
{
    class BpmSelection;

    class BpmSelectionObserver
        : public infra::Observer<BpmSelectionObserver, BpmSelection>
    {
    public:
        using infra::Observer<BpmSelectionObserver, BpmSelection>::Observer;

        virtual void BpmSelected(uint16_t bpm) = 0;
        virtual void SingleStep(int steps) = 0;
        virtual void MultiStep(int steps) = 0;
        virtual void ToggleStart() = 0;
    };

    class BpmSelection
        : public infra::Subject<BpmSelectionObserver>
    {
    public:
        virtual void SetBpm(uint16_t bpm) = 0;
    };

    class ViewBpm
        : public services::View
        , public services::TouchRecipient
        , public BpmSelection
        , public BeatControllerObserver
    {
    public:
        ViewBpm(BeatController& controller);

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

        // Implementation of BpmSelection
        virtual void SetBpm(uint16_t bpm) override;

        // Implementation of BeatControllerObserver
        virtual void PrepareBeat() override;

    private:
        void DrawSprocket(hal::Canvas& canvas, infra::Region boundingRegion, int sprocket) const;
        infra::Point SprocketCentre(int i) const;

    private:
        infra::BoundedString::WithStorage<4> bpm;
        uint16_t proportion;
        infra::TimerRepeating rollOff;

        infra::Colour backgroundColour = infra::Colour::white;

        enum class TouchMode
            : uint8_t
        {
            idle,
            wheel,
            bpmSingle,
            bpmMulti
        } mode;

        infra::Optional<infra::Point> startTouch;
        bool touchRemainedAtStart;
        infra::Optional<int> selectedSprocket;
        int stepsReported;

        static const std::array<uint16_t, 8> sprocketValues;
    };
}

#endif
