#ifndef METRONOME_BPM_SELECTION_HPP
#define METRONOME_BPM_SELECTION_HPP

#include "infra/util/Observer.hpp"
#include <cstdint>

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
}

#endif
