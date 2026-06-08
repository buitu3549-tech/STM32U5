#include "system_stm32u5xx.h"
#include "stm32u575xx.h"

uint32_t SystemCoreClock = 16000000U;

/* ================================================================
 *  Helper — access an RCC register at a given byte offset from RCC_BASE
 * ================================================================ */
static __inline volatile uint32_t *rcc_reg(uint32_t offset) {
    return (volatile uint32_t *)(RCC_BASE + offset);
}

/* ================================================================
 *  SystemInit — called from startup before main()
 *  Enables FPU, sets NVIC priority grouping, keeps HSI as default clock.
 * ================================================================ */
void SystemInit(void) {
    /* Enable FPU (CP10/CP11 full access) */
    FPU->CPACR |= FPU_CPACR_CP10_FULL | FPU_CPACR_CP11_FULL;
    __DSB();
    __ISB();

    /* 4-bit preemption priority, 0-bit sub-priority */
    SCB->AIRCR = SCB_AIRCR_VECTKEY | (0x3UL << SCB_AIRCR_PRIGROUP_Pos);
    __DSB();
    __ISB();

    SystemCoreClock = HSI_VALUE;
}

/* ================================================================
 *  HSI-only 16 MHz — fallback safe clock
 * ================================================================ */
void SystemClock_HSI_16M(void) {
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

    RCC->CFGR1 = (RCC->CFGR1 & ~RCC_CFGR1_SW_Mask) | RCC_CFGR1_SW_HSI;
    while ((RCC->CFGR1 & RCC_CFGR1_SWS_Mask) != RCC_CFGR1_SWS_HSI) {}

    SystemCoreClock = HSI_VALUE;
}

/* ================================================================ *
 *  PLL 160 MHz — HSI 16 MHz → PLL1 (/1, ×20, /2) → 160 MHz SYSCLK
 *
 *  Configures Flash wait states for 160 MHz (Vcore Range 1).
 *  Adjust FLASH_ACR latency bits per RM0456 §3.3.4 for your Vcore range.
 * ================================================================ */
void SystemClock_PLL_160M(void) {
    /* 1. Ensure HSI is on */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

    /* 2. PLL1CFGR: source=HSI, M=1, N=20, wide VCO range */
    RCC->PLL1CFGR = RCC_PLL1CFGR_PLL1SRC_HSI
                  | RCC_PLL1CFGR_PLL1M(1)
                  | RCC_PLL1CFGR_PLL1N(20)
                  | RCC_PLL1CFGR_PLL1VCOSEL;

    /* 3. PLL1DIVR: R=2, Q=2, P=2 */
    RCC->PLL1DIVR = RCC_PLL1DIVR_PLL1R(2)
                  | RCC_PLL1DIVR_PLL1Q(2)
                  | RCC_PLL1DIVR_PLL1P(2);

    /* 4. Enable PLL1 and wait for lock */
    RCC->CR |= RCC_CR_PLL1ON;
    while (!(RCC->CR & RCC_CR_PLL1RDY)) {}

    /* 5. Flash latency: 4 wait states for 160 MHz (verify per your Vcore range) */
    FLASH_CTRL->ACR = (FLASH_CTRL->ACR & ~0x7UL) | 4;
    FLASH_CTRL->ACR |= FLASH_ACR_PRFTEN;

    /* 6. Switch system clock to PLL1 */
    RCC->CFGR1 = (RCC->CFGR1 & ~RCC_CFGR1_SW_Mask) | RCC_CFGR1_SW_PLL1;
    while ((RCC->CFGR1 & RCC_CFGR1_SWS_Mask) != RCC_CFGR1_SWS_PLL1) {}

    SystemCoreClock = SYSCLK_VALUE;
}
