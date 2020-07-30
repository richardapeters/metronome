#ifndef METRONOME_WM8994_HPP
#define METRONOME_WM8994_HPP

#include "hal/interfaces/I2c.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/util/Endian.hpp"

namespace application
{
    class Wm8994
    {
    public:
        Wm8994(hal::I2cMaster& i2c, const infra::Function<void()>& onInitialized, hal::I2cAddress address = hal::I2cAddress(0x1a));

    private:
        void ExecuteCommands(infra::MemoryRange<const std::pair<uint16_t, uint16_t>> commands, const infra::Function<void()>& onDone);
        void ExecuteCommands();
        void ExecuteCommand(const std::pair<uint16_t, uint16_t>& command);

    private:
        hal::I2cMaster& i2c;
        hal::I2cAddress address;
        infra::Function<void()> onInitialized;

        infra::TimerSingleShot timer;

        infra::MemoryRange<const std::pair<uint16_t, uint16_t>> commands;
        infra::Function<void()> onDone;

        struct
        {
            infra::BigEndian<uint16_t> reg;
            infra::BigEndian<uint16_t> value;
        } commandBuffer;
    };
}

#endif
