/*
 * startup_stm32u575xx.s — Cortex-M33 startup
 * Initialises .data / .bss, calls SystemInit, then main.
 */
    .syntax unified
    .cpu    cortex-m33
    .fpu    fpv5-sp-d16
    .thumb

    .global g_pfnVectors
    .global Default_Handler

    .global Reset_Handler
    .global NMI_Handler
    .global HardFault_Handler
    .global MemManage_Handler
    .global BusFault_Handler
    .global UsageFault_Handler
    .global SecureFault_Handler
    .global SVC_Handler
    .global DebugMon_Handler
    .global PendSV_Handler
    .global SysTick_Handler

    /* External IRQs */
    .global WWDG_IRQHandler
    .global PVD_PVM_IRQHandler
    .global RTC_IRQHandler
    .global FLASH_IRQHandler
    .global RCC_IRQHandler
    .global EXTI0_IRQHandler
    .global EXTI1_IRQHandler
    .global EXTI2_IRQHandler
    .global EXTI3_IRQHandler
    .global EXTI4_IRQHandler
    .global EXTI5_IRQHandler
    .global EXTI6_IRQHandler
    .global EXTI7_IRQHandler
    .global EXTI8_IRQHandler
    .global EXTI9_IRQHandler
    .global EXTI10_IRQHandler
    .global EXTI11_IRQHandler
    .global EXTI12_IRQHandler
    .global EXTI13_IRQHandler
    .global EXTI14_IRQHandler
    .global EXTI15_IRQHandler
    .global DMA1_CH1_IRQHandler
    .global DMA1_CH2_IRQHandler
    .global DMA1_CH3_IRQHandler
    .global DMA1_CH4_IRQHandler
    .global DMA1_CH5_IRQHandler
    .global DMA1_CH6_IRQHandler
    .global DMA1_CH7_IRQHandler
    .global DMA1_CH8_IRQHandler
    .global DMA2_CH1_IRQHandler
    .global DMA2_CH2_IRQHandler
    .global DMA2_CH3_IRQHandler
    .global DMA2_CH4_IRQHandler
    .global DMA2_CH5_IRQHandler
    .global DMA2_CH6_IRQHandler
    .global DMA2_CH7_IRQHandler
    .global DMA2_CH8_IRQHandler
    .global ADC1_IRQHandler
    .global ADC4_IRQHandler
    .global TIM1_IRQHandler
    .global TIM2_IRQHandler
    .global TIM3_IRQHandler
    .global TIM4_IRQHandler
    .global TIM5_IRQHandler
    .global TIM6_IRQHandler
    .global TIM7_IRQHandler
    .global TIM8_IRQHandler
    .global TIM15_IRQHandler
    .global TIM16_IRQHandler
    .global TIM17_IRQHandler
    .global I2C1_IRQHandler
    .global I2C2_IRQHandler
    .global I2C3_IRQHandler
    .global I2C4_IRQHandler
    .global SPI1_IRQHandler
    .global SPI2_IRQHandler
    .global SPI3_IRQHandler
    .global USART1_IRQHandler
    .global USART2_IRQHandler
    .global USART3_IRQHandler
    .global UART4_IRQHandler
    .global LPUART1_IRQHandler
    .global LPTIM1_IRQHandler
    .global LPTIM2_IRQHandler
    .global LPTIM3_IRQHandler
    .global LPTIM4_IRQHandler
    .global SAI1_IRQHandler
    .global SAI2_IRQHandler
    .global SDMMC1_IRQHandler
    .global FDCAN1_0_IRQHandler
    .global FDCAN1_1_IRQHandler
    .global FDCAN2_0_IRQHandler
    .global FDCAN2_1_IRQHandler
    .global OCTOSPI1_IRQHandler
    .global USB_FS_IRQHandler
    .global UCPD1_IRQHandler
    .global CRS_IRQHandler
    .global RNG_IRQHandler
    .global FPU_IRQHandler
    .global TSC_IRQHandler
    .global DMA2D_IRQHandler
    .global GFXMMU_IRQHandler
    .global HASH_IRQHandler
    .global ICACHE_IRQHandler
    .global DCACHE1_IRQHandler

    .section .isr_vector, "a", %progbits
    .type   g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word SecureFault_Handler
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler
    .word PVD_PVM_IRQHandler
    .word RTC_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word EXTI5_IRQHandler
    .word EXTI6_IRQHandler
    .word EXTI7_IRQHandler
    .word EXTI8_IRQHandler
    .word EXTI9_IRQHandler
    .word EXTI10_IRQHandler
    .word EXTI11_IRQHandler
    .word EXTI12_IRQHandler
    .word EXTI13_IRQHandler
    .word EXTI14_IRQHandler
    .word EXTI15_IRQHandler
    .word DMA1_CH1_IRQHandler
    .word DMA1_CH2_IRQHandler
    .word DMA1_CH3_IRQHandler
    .word DMA1_CH4_IRQHandler
    .word DMA1_CH5_IRQHandler
    .word DMA1_CH6_IRQHandler
    .word DMA1_CH7_IRQHandler
    .word DMA1_CH8_IRQHandler
    .word DMA2_CH1_IRQHandler
    .word DMA2_CH2_IRQHandler
    .word DMA2_CH3_IRQHandler
    .word DMA2_CH4_IRQHandler
    .word DMA2_CH5_IRQHandler
    .word DMA2_CH6_IRQHandler
    .word DMA2_CH7_IRQHandler
    .word DMA2_CH8_IRQHandler
    .word ADC1_IRQHandler
    .word ADC4_IRQHandler
    .word TIM1_IRQHandler
    .word TIM2_IRQHandler
    .word TIM3_IRQHandler
    .word TIM4_IRQHandler
    .word TIM5_IRQHandler
    .word TIM6_IRQHandler
    .word TIM7_IRQHandler
    .word TIM8_IRQHandler
    .word TIM15_IRQHandler
    .word TIM16_IRQHandler
    .word TIM17_IRQHandler
    .word I2C1_IRQHandler
    .word I2C2_IRQHandler
    .word I2C3_IRQHandler
    .word I2C4_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word SPI3_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word USART3_IRQHandler
    .word UART4_IRQHandler
    .word LPUART1_IRQHandler
    .word LPTIM1_IRQHandler
    .word LPTIM2_IRQHandler
    .word LPTIM3_IRQHandler
    .word LPTIM4_IRQHandler
    .word SAI1_IRQHandler
    .word SAI2_IRQHandler
    .word SDMMC1_IRQHandler
    .word FDCAN1_0_IRQHandler
    .word FDCAN1_1_IRQHandler
    .word FDCAN2_0_IRQHandler
    .word FDCAN2_1_IRQHandler
    .word OCTOSPI1_IRQHandler
    .word USB_FS_IRQHandler
    .word UCPD1_IRQHandler
    .word CRS_IRQHandler
    .word RNG_IRQHandler
    .word FPU_IRQHandler
    .word TSC_IRQHandler
    .word DMA2D_IRQHandler
    .word GFXMMU_IRQHandler
    .word HASH_IRQHandler
    .word ICACHE_IRQHandler
    .word DCACHE1_IRQHandler

    .section .text.Reset_Handler
    .weak   Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    ldr    sp, =_estack

    ldr    r0, =_sdata
    ldr    r1, =_edata
    ldr    r2, =_sidata
    cmp    r0, r1
    beq    .L_zero_bss
.L_copy_data:
    ldr    r3, [r2], #4
    str    r3, [r0], #4
    cmp    r0, r1
    bne    .L_copy_data

.L_zero_bss:
    ldr    r0, =_sbss
    ldr    r1, =_ebss
    movs   r2, #0
    cmp    r0, r1
    beq    .L_call_init
.L_zero_bss_loop:
    str    r2, [r0], #4
    cmp    r0, r1
    bne    .L_zero_bss_loop

.L_call_init:
    bl     SystemInit
    bl     main
.L_loop:
    b      .L_loop

    .section .text.Default_Handler, "ax", %progbits
Default_Handler:
.L_infinite:
    b      .L_infinite

    .macro  weak_alias name
    .weak   \name
    .thumb_set \name, Default_Handler
    .endm

    weak_alias NMI_Handler
    weak_alias HardFault_Handler
    weak_alias MemManage_Handler
    weak_alias BusFault_Handler
    weak_alias UsageFault_Handler
    weak_alias SecureFault_Handler
    weak_alias SVC_Handler
    weak_alias DebugMon_Handler
    weak_alias PendSV_Handler
    weak_alias SysTick_Handler
    weak_alias WWDG_IRQHandler
    weak_alias PVD_PVM_IRQHandler
    weak_alias RTC_IRQHandler
    weak_alias FLASH_IRQHandler
    weak_alias RCC_IRQHandler
    weak_alias EXTI0_IRQHandler
    weak_alias EXTI1_IRQHandler
    weak_alias EXTI2_IRQHandler
    weak_alias EXTI3_IRQHandler
    weak_alias EXTI4_IRQHandler
    weak_alias EXTI5_IRQHandler
    weak_alias EXTI6_IRQHandler
    weak_alias EXTI7_IRQHandler
    weak_alias EXTI8_IRQHandler
    weak_alias EXTI9_IRQHandler
    weak_alias EXTI10_IRQHandler
    weak_alias EXTI11_IRQHandler
    weak_alias EXTI12_IRQHandler
    weak_alias EXTI13_IRQHandler
    weak_alias EXTI14_IRQHandler
    weak_alias EXTI15_IRQHandler
    weak_alias DMA1_CH1_IRQHandler
    weak_alias DMA1_CH2_IRQHandler
    weak_alias DMA1_CH3_IRQHandler
    weak_alias DMA1_CH4_IRQHandler
    weak_alias DMA1_CH5_IRQHandler
    weak_alias DMA1_CH6_IRQHandler
    weak_alias DMA1_CH7_IRQHandler
    weak_alias DMA1_CH8_IRQHandler
    weak_alias DMA2_CH1_IRQHandler
    weak_alias DMA2_CH2_IRQHandler
    weak_alias DMA2_CH3_IRQHandler
    weak_alias DMA2_CH4_IRQHandler
    weak_alias DMA2_CH5_IRQHandler
    weak_alias DMA2_CH6_IRQHandler
    weak_alias DMA2_CH7_IRQHandler
    weak_alias DMA2_CH8_IRQHandler
    weak_alias ADC1_IRQHandler
    weak_alias ADC4_IRQHandler
    weak_alias TIM1_IRQHandler
    weak_alias TIM2_IRQHandler
    weak_alias TIM3_IRQHandler
    weak_alias TIM4_IRQHandler
    weak_alias TIM5_IRQHandler
    weak_alias TIM6_IRQHandler
    weak_alias TIM7_IRQHandler
    weak_alias TIM8_IRQHandler
    weak_alias TIM15_IRQHandler
    weak_alias TIM16_IRQHandler
    weak_alias TIM17_IRQHandler
    weak_alias I2C1_IRQHandler
    weak_alias I2C2_IRQHandler
    weak_alias I2C3_IRQHandler
    weak_alias I2C4_IRQHandler
    weak_alias SPI1_IRQHandler
    weak_alias SPI2_IRQHandler
    weak_alias SPI3_IRQHandler
    weak_alias USART1_IRQHandler
    weak_alias USART2_IRQHandler
    weak_alias USART3_IRQHandler
    weak_alias UART4_IRQHandler
    weak_alias LPUART1_IRQHandler
    weak_alias LPTIM1_IRQHandler
    weak_alias LPTIM2_IRQHandler
    weak_alias LPTIM3_IRQHandler
    weak_alias LPTIM4_IRQHandler
    weak_alias SAI1_IRQHandler
    weak_alias SAI2_IRQHandler
    weak_alias SDMMC1_IRQHandler
    weak_alias FDCAN1_0_IRQHandler
    weak_alias FDCAN1_1_IRQHandler
    weak_alias FDCAN2_0_IRQHandler
    weak_alias FDCAN2_1_IRQHandler
    weak_alias OCTOSPI1_IRQHandler
    weak_alias USB_FS_IRQHandler
    weak_alias UCPD1_IRQHandler
    weak_alias CRS_IRQHandler
    weak_alias RNG_IRQHandler
    weak_alias FPU_IRQHandler
    weak_alias TSC_IRQHandler
    weak_alias DMA2D_IRQHandler
    weak_alias GFXMMU_IRQHandler
    weak_alias HASH_IRQHandler
    weak_alias ICACHE_IRQHandler
    weak_alias DCACHE1_IRQHandler
