#include "infra/event/EventDispatcher.hpp"
#include "metronome/main_stm32f746/LowPowerTimer.hpp"
#include "services/tracer/GlobalTracer.hpp"

namespace hal
{
    LowPowerTimer::LowPowerTimer()
    {
        __HAL_RCC_PWR_CLK_ENABLE();
        __HAL_RCC_LPTIM1_CLK_ENABLE();

        RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~RCC_DCKCFGR2_LPTIM1SEL) | RCC_LPTIM1CLKSOURCE_LSE;
        PWR->CR1 |= PWR_CR1_DBP;
        __DSB();
        RCC->BDCR |= RCC_BDCR_LSEON;

        LPTIM1->CR = LPTIM_CR_ENABLE;
        LPTIM1->CFGR = 0;
        LPTIM1->ARR = reload;
        LPTIM1->IER = LPTIM_IER_ARRMIE;
    }

    LowPowerTimer::~LowPowerTimer()
    {
        LPTIM1->CR = 0;
        __HAL_RCC_LPTIM1_CLK_DISABLE();
    }

    void LowPowerTimer::SetReload(uint32_t reload)
    {
        LPTIM1->ARR = reload;
        this->reload = reload;
    }

    void LowPowerTimer::Start()
    {
        previousReload = reload;
        LPTIM1->CR = LPTIM_CR_CNTSTRT | LPTIM_CR_ENABLE;
    }

    void LowPowerTimer::Stop()
    {
        LPTIM1->CR = 0;
        LPTIM1->CR = LPTIM_CR_ENABLE;
    }

    infra::TimePoint LowPowerTimer::Now() const
    {
        return now + std::chrono::microseconds(LPTIM1->CR * 1000000 / 32768);
    }

    void LowPowerTimer::Reload()
    {
        LPTIM1->ICR = LPTIM_ICR_ARRMCF;
        now += std::chrono::microseconds(previousReload * 1000000 / 32768);
        previousReload = reload;
    }
}
