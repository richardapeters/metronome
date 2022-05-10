#include "metronome/application/BpmSelectionInteractor.hpp"

namespace application
{
    BpmSelectionInteractor::BpmSelectionInteractor(BpmSelection& subject, BeatController& beatController)
        : BpmSelectionObserver(subject)
        , beatController(beatController)
    {
        Update();
    }

    void BpmSelectionInteractor::BpmSelected(uint16_t selectedBpm)
    {
        bpm = selectedBpm;
        Update();
    }

    void BpmSelectionInteractor::Update()
    {
        bpm = beatController.MakeBpmValid(bpm);

        Subject().SetBpm(bpm);
        beatController.SetBpm(bpm);
    }
}
