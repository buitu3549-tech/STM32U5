#ifndef SYSTEM_STM32U5XX_H
#define SYSTEM_STM32U5XX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;

void SystemInit(void);
void SystemClock_HSI_16M(void);
void SystemClock_PLL_160M(void);

#ifdef __cplusplus
}
#endif

#endif
