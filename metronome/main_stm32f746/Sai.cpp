#include "metronome/main_stm32f746/Sai.hpp"

namespace main_
{
    Sai::Sai(hal::DmaStm& dma)
        : controller(2, hal::saiA, mclock, sck, sd, fs, dma)
    {
        ConfigureSaiClock();
    }

    void Sai::ConfigureSaiClock()
    {
        RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
        HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

        // Configure PLLSAI prescalers
        // PLLI2S_VCO: VCO_429M
        // SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
        // SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 214.5/19 = 11.289 Mhz
        RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
        RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
        RCC_ExCLKInitStruct.PLLI2SDivQ = 19;
        HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
    }
}
