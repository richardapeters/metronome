#include "metronome/application/Wm8994.hpp"

namespace application
{
    namespace
    {
        const std::array<std::pair<uint16_t, uint16_t>, 6> initCommands1{ {
            { 0x0, 0x1 },           // Soft restart
            { 0x102, 0x0003 },      // Errata
            { 0x817, 0x0000 },
            { 0x102, 0x0000 },
            { 0x39, 0x006C },       // Enable VMID soft start (fast), Start-up Bias Current Enabled
            { 0x01, 0x0003 }        // Enable bias generator, Enable VMID
        } };

        const std::array<std::pair<uint16_t, uint16_t>, 19> initCommands2{ {
            { 0x05, 0x0303 },       // Enable DAC1 (Left), Enable DAC1 (Right), Disable DAC2 (Left), Disable DAC2 (Right), Enable AIF1DAC1 (Left) input path (AIF1, Timeslot 0), Enable AIF1DAC1 (Right) input path (AIF1, Timeslot 0)
            { 0x601, 0x0001 },      // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
            { 0x602, 0x0001 },      // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
            { 0x604, 0x0000 },      // Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path
            { 0x605, 0x0000 },      // Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path
            { 0x210, 0x0073 },      // AIF1 Sample Rate = 44.1 (KHz), ratio=256
            { 0x300, 0x4010 },      // AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value)
            { 0x302, 0x0000 },      // slave mode
            { 0x208, 0x000A },      // Enable the DSP processing clock for AIF1, Enable the core clock
            { 0x200, 0x0001 },      // Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin
            // Analog output configuration
            { 0x03, 0x0300 },       // Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL
            { 0x22, 0x0000 },       // Left Speaker Mixer Volume = 0dB
            { 0x23, 0x0000 },       // Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)
            { 0x36, 0x0300 },       // Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path, Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path
            { 0x01, 0x3003 },       // Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR
            // Headphone/Speaker enable
            { 0x51, 0x0005 },       // Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0
            { 0x01, 0x3303 },       // Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages
            { 0x60, 0x0022 },       // Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages
            { 0x4C, 0x9F25 },       // Enable Charge Pump
        } };

        const std::array<std::pair<uint16_t, uint16_t>, 4> initCommands3{ {
            { 0x2D, 0x0001 },       // Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path
            { 0x2E, 0x0001 },       // Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path
            { 0x03, 0x0330 },       // Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR), idem for SPKOUTL and SPKOUTR
            { 0x54, 0x0033 },       // Enable DC Servo and trigger start-up mode on left and right channels
        } };

        const std::array<std::pair<uint16_t, uint16_t>, 6> initCommands4{ {
            { 0x60, 0x00EE },       // Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps
            { 0x610, 0x00C0 },      // Unmute DAC1 ( Left)
            { 0x611, 0x00C0 },      // Unmute DAC1 (Right)
            { 0x420, 0x0000 },      // Unmute the AIF1 Timeslot 0 DAC path
            { 0x1c, 0x3f | 0x140 }, // Left Headphone Volume
            { 0x1d, 0x3f | 0x140 }, // Right Headphone Volume
        } };
    }

    Wm8994::Wm8994(hal::I2cMaster& i2c, const infra::Function<void()>& onInitialized, hal::I2cAddress address)
        : i2c(i2c)
        , address(address)
        , onInitialized(onInitialized)
    {
        ExecuteCommands(initCommands1, [this]()
        {
            timer.Start(std::chrono::milliseconds(50), [this]()
            {
                ExecuteCommands(initCommands2, [this]()
                {
                    timer.Start(std::chrono::milliseconds(15), [this]()
                    {
                        ExecuteCommands(initCommands3, [this]()
                        {
                            timer.Start(std::chrono::milliseconds(250), [this]()
                            {
                                ExecuteCommands(initCommands4, this->onInitialized);
                            });
                        });
                    });
                });
            });
        });
    }

    void Wm8994::ExecuteCommands(infra::MemoryRange<const std::pair<uint16_t, uint16_t>> commands, const infra::Function<void()>& onDone)
    {
        this->commands = commands;
        this->onDone = onDone;
        ExecuteCommands();
    }

    void Wm8994::ExecuteCommands()
    {
        if (commands.empty())
            onDone();
        else
        {
            auto& command = commands.front();
            commands.pop_front();
            ExecuteCommand(command);
        }
    }

    void Wm8994::ExecuteCommand(const std::pair<uint16_t, uint16_t>& command)
    {
        commandBuffer.reg = command.first;
        commandBuffer.value = command.second;

        i2c.SendData(address, infra::MakeByteRange(commandBuffer), hal::Action::stop, [this, &command](hal::Result, uint32_t numberOfBytesSent)
        {
            ExecuteCommands();
        });
    }
}
