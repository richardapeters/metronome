#ifndef METRONOME_BPM_SELECTION_INTERACTOR_HPP
#define METRONOME_BPM_SELECTION_INTERACTOR_HPP

#include "metronome/interfaces/BeatController.hpp"
#include "metronome/interfaces/BpmSelection.hpp"

namespace application
{
    class BpmSelectionInteractor
        : public BpmSelectionObserver
    {
    public:
        BpmSelectionInteractor(BpmSelection& subject, BeatController& beatController);

        virtual void BpmSelected(uint16_t selectedBpm) override;

    private:
        void Update();

    private:
        BeatController& beatController;
        uint16_t bpm = 120;
    };
}

#endif
