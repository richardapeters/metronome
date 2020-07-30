#ifndef METRONOME_BPM_SELECTION_INTERACTOR_HPP
#define METRONOME_BPM_SELECTION_INTERACTOR_HPP

#include "metronome/application/ViewBpm.hpp"

namespace application
{
    class BpmSelectionInteractor
        : public BpmSelectionObserver
    {
    public:
        BpmSelectionInteractor(BpmSelection& subject, BeatController& beatController);

        virtual void BpmSelected(uint16_t selectedBpm) override;
        virtual void SingleStep(int steps) override;
        virtual void MultiStep(int steps) override;
        virtual void ToggleStart() override;

    private:
        void Update();

    private:
        BeatController& beatController;
        uint16_t bpm = 120;
    };
}

#endif
