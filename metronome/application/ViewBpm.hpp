#ifndef METRONOME_VIEW_BPM_HPP
#define METRONOME_VIEW_BPM_HPP

#include "infra/util/BoundedVector.hpp"
#include "infra/util/Observer.hpp"
#include "infra/timer/Timer.hpp"
#include "metronome/application/BeatController.hpp"
#include "metronome/application/ViewSprocket.hpp"
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
    };

    class BpmSelection
        : public infra::Subject<BpmSelectionObserver>
    {
    public:
        virtual void SetBpm(uint16_t bpm) = 0;
    };

    struct Note
    {
        uint16_t moment;
        uint8_t pitch;
    };

    class Notes;

    class NotesObserver
        : public infra::SingleObserver<NotesObserver, Notes>
    {
    public:
        using infra::SingleObserver<NotesObserver, Notes>::SingleObserver;

        virtual void NotesChanged(infra::MemoryRange<const Note> newNotes) = 0;
    };

    class Notes
        : public infra::Subject<NotesObserver>
    {};

    class ViewTimeline
        : public services::View
        , public NotesObserver
    {
    public:
        using NotesObserver::NotesObserver;

        // Implementation of View
        virtual void Paint(hal::Canvas& canvas, infra::Region boundingRegion) override;
        virtual void ViewRegionChanged() override;

        // Implementation of NotesObserver
        virtual void NotesChanged(infra::MemoryRange<const Note> newNotes) override;

    private:
        infra::BoundedVector<std::pair<infra::Point, infra::Point>>::WithMaxSize<16> lines;
        infra::BoundedVector<infra::Point>::WithMaxSize<128> notes;
    };

    class ViewBpm
        : public services::View
        , public services::TouchRecipient
        , public BpmSelection
        , public BeatControllerObserver
    {
    public:
        ViewBpm(BeatController& controller, Notes& notes);

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

        // Implementation of BeatControllerObserver
        virtual void BeatOn() override;
        virtual void BeatOff() override;

    private:
        void Select(uint16_t value);

    private:
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
