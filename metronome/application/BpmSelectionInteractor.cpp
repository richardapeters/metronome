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

    void BpmSelectionInteractor::SingleStep(int steps)
    {
        bpm += steps;
        Update();
    }

    void BpmSelectionInteractor::MultiStep(int steps)
    {
        if (steps > 0)
            bpm += 10 * steps - bpm % 10;
        else if (steps < 0)
            bpm += 10 * steps - (bpm - 1) % 10 + 9;

        Update();
    }

    void BpmSelectionInteractor::ToggleStart()
    {
        if (beatController.Running())
            beatController.Stop();
        else
            beatController.Start();
    }

    void BpmSelectionInteractor::Update()
    {
        bpm = std::min(300, std::max(10, bpm + 0));

        Subject().SetBpm(bpm);
        beatController.SetBpm(bpm);
    }
}
