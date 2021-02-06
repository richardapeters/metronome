#ifndef METRONOME_BEAT_CONTROLLER_HPP
#define METRONOME_BEAT_CONTROLLER_HPP

#include "infra/util/Observer.hpp"
#include "infra/timer/Timer.hpp"

namespace application
{
    class BeatTimer
    {
    protected:
        BeatTimer() = default;
        BeatTimer(const BeatTimer& other) = delete;
        BeatTimer& operator=(const BeatTimer& other) = delete;
        ~BeatTimer() = default;

    public:
        virtual void Start(uint16_t bpm) = 0;
        virtual void Stop() = 0;
    };

    class BeatController;

    class BeatControllerObserver
        : public infra::Observer<BeatControllerObserver, BeatController>
    {
    public:
        using infra::Observer<BeatControllerObserver, BeatController>::Observer;

        virtual void PrepareBeat() = 0;
    };

    class BeatController
        : public infra::Subject<BeatControllerObserver>
    {
    public:
        virtual void SetBpm(uint16_t newBpm) = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool Running() const = 0;
    };

    class BeatControllerImpl
        : public BeatController
    {
    public:
        BeatControllerImpl(BeatTimer& beatTimer);

        virtual void SetBpm(uint16_t newBpm) override;
        virtual void Start() override;
        virtual void Stop() override;
        virtual bool Running() const override;

    private:
        void Beat();

    private:
        BeatTimer& beatTimer;
        infra::TimerRepeating beat;
        uint16_t bpm;
    };
}

#endif
