/* ================================================================
 *  stm32u575xx.h — CMSIS-style device header for STM32U575xx
 *
 *  Cortex-M33, 2 MB Flash, 786 KB SRAM, up to 160 MHz.
 *  Register addresses per RM0456 (STM32U5 reference manual).
 * ================================================================ */
#ifndef STM32U575XX_H
#define STM32U575XX_H

#include <stdint.h>

/* ================================================================
 *  Core / System peripherals (Cortex-M33)
 * ================================================================ */
#define FLASH_BASE            0x08000000UL
#define SRAM1_BASE            0x20000000UL
#define PERIPH_BASE           0x40000000UL
#define AHB1_PERIPH_BASE      (PERIPH_BASE + 0x00020000UL)
#define AHB2_PERIPH_BASE      (PERIPH_BASE + 0x02020000UL)
#define AHB3_PERIPH_BASE      (PERIPH_BASE + 0x06020000UL)
#define APB1_PERIPH_BASE      (PERIPH_BASE + 0x06010000UL)
#define APB2_PERIPH_BASE      (PERIPH_BASE + 0x06010000UL)

/* Cortex-M33 internal */
#define SCS_BASE              0xE000E000UL
#define NVIC_BASE             (SCS_BASE + 0x0100UL)
#define SCB_BASE              (SCS_BASE + 0x0D00UL)
#define SysTick_BASE          (SCS_BASE + 0x0010UL)
#define MPU_BASE              (SCS_BASE + 0x0D90UL)
#define FPU_BASE              (SCS_BASE + 0x0F30UL)

/* ================================================================
 *  Peripheral base addresses
 * ================================================================ */

/* AHB2 — GPIO */
#define GPIOA_BASE            (AHB2_PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB2_PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB2_PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE            (AHB2_PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE            (AHB2_PERIPH_BASE + 0x1000UL)
#define GPIOF_BASE            (AHB2_PERIPH_BASE + 0x1400UL)
#define GPIOG_BASE            (AHB2_PERIPH_BASE + 0x1800UL)
#define GPIOH_BASE            (AHB2_PERIPH_BASE + 0x1C00UL)
#define GPIOI_BASE            (AHB2_PERIPH_BASE + 0x2000UL)

/* APB2 — USART1 */
#define USART1_BASE           (APB2_PERIPH_BASE + 0x3800UL)

/* APB1 — USART2/3, UART4 */
#define USART2_BASE           (APB1_PERIPH_BASE + 0x3C00UL)
#define USART3_BASE           (APB1_PERIPH_BASE + 0x4000UL)
#define UART4_BASE            (APB1_PERIPH_BASE + 0x4400UL)

/* AHB3 — RCC, PWR, PKA, AES, RNG */
#define RCC_BASE              (AHB3_PERIPH_BASE + 0x0C00UL)
#define PWR_BASE              (AHB3_PERIPH_BASE + 0x0800UL)

/* FLASH controller (on AHB1) */
#define FLASH_CTRL_BASE       (AHB1_PERIPH_BASE + 0x2000UL)

/* ================================================================
 *  Register structures
 * ================================================================ */

/* --- NVIC --- */
typedef struct {
    volatile uint32_t ISER[16];
    volatile uint32_t RES0[16];
    volatile uint32_t ICER[16];
    volatile uint32_t RES1[16];
    volatile uint32_t ISPR[16];
    volatile uint32_t RES2[16];
    volatile uint32_t ICPR[16];
    volatile uint32_t RES3[16];
    volatile uint32_t IABR[16];
    volatile uint32_t RES4[48];
    volatile uint8_t  IP[512];
} NVIC_TypeDef;

/* --- SCB --- */
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint8_t  SHPR[12];
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
} SCB_TypeDef;

/* --- SysTick --- */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

/* --- FPU (CPACR for enabling) --- */
typedef struct {
    volatile uint32_t CPACR;
} FPU_TypeDef;

/* --- GPIO --- */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;
} GPIO_TypeDef;

/* --- RCC (STM32U5 — key registers only; expand as needed) --- */
typedef struct {
    volatile uint32_t CR;            /* 0x00 */
    volatile uint32_t CFGR1;         /* 0x04 */
    volatile uint32_t CFGR2;         /* 0x08 */
    volatile uint32_t CFGR3;         /* 0x0C */
    volatile uint32_t RES0[2];
    volatile uint32_t PLL1CFGR;      /* 0x18 */
    volatile uint32_t RES1;
    volatile uint32_t RES2[4];
    volatile uint32_t PLL1DIVR;      /* 0x28 */
} RCC_TypeDef;

/* --- USART --- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
    volatile uint32_t PRESC;
} USART_TypeDef;

/* --- FLASH --- */
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t ECCR;
    volatile uint32_t RES0[2];
    volatile uint32_t OPTR;
    volatile uint32_t PCROP1SR;
    volatile uint32_t PCROP1ER;
    volatile uint32_t RES1[4];
    volatile uint32_t PCROP2SR;
    volatile uint32_t PCROP2ER;
} FLASH_TypeDef;

/* --- PWR (U5 specific — key registers) --- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
} PWR_TypeDef;

/* ================================================================
 *  Peripheral pointers
 * ================================================================ */
#define GPIOA       ((GPIO_TypeDef   *) GPIOA_BASE)
#define GPIOB       ((GPIO_TypeDef   *) GPIOB_BASE)
#define GPIOC       ((GPIO_TypeDef   *) GPIOC_BASE)
#define GPIOD       ((GPIO_TypeDef   *) GPIOD_BASE)
#define GPIOE       ((GPIO_TypeDef   *) GPIOE_BASE)
#define GPIOF       ((GPIO_TypeDef   *) GPIOF_BASE)
#define GPIOG       ((GPIO_TypeDef   *) GPIOG_BASE)
#define USART1      ((USART_TypeDef  *) USART1_BASE)
#define USART2      ((USART_TypeDef  *) USART2_BASE)
#define USART3      ((USART_TypeDef  *) USART3_BASE)
#define RCC         ((RCC_TypeDef    *) RCC_BASE)
#define FLASH_CTRL  ((FLASH_TypeDef  *) FLASH_CTRL_BASE)
#define PWR         ((PWR_TypeDef    *) PWR_BASE)
#define NVIC        ((NVIC_TypeDef   *) NVIC_BASE)
#define SCB         ((SCB_TypeDef    *) SCB_BASE)
#define SysTick     ((SysTick_TypeDef*) SysTick_BASE)
#define FPU         ((FPU_TypeDef    *) FPU_BASE)

/* ================================================================
 *  RCC register offsets (beyond the RCC_TypeDef struct)
 * ================================================================ */
#define RCC_AHB2ENR_OFFSET    0xA4
#define RCC_AHB3ENR_OFFSET    0xA8
#define RCC_APB1ENR1_OFFSET   0xB0
#define RCC_APB1ENR2_OFFSET   0xB4
#define RCC_APB2ENR_OFFSET    0xB8
#define RCC_APB3ENR_OFFSET    0xBC

/* ================================================================
 *  Bit definitions
 * ================================================================ */

/* --- RCC CR --- */
#define RCC_CR_PLL1RDY        (1UL << 25)
#define RCC_CR_PLL1ON         (1UL << 24)
#define RCC_CR_HSERDY         (1UL << 17)
#define RCC_CR_HSEON          (1UL << 16)
#define RCC_CR_HSIRDY         (1UL << 10)
#define RCC_CR_HSION          (1UL << 8)

/* --- RCC CFGR1 --- */
#define RCC_CFGR1_SWS_Pos     0
#define RCC_CFGR1_SWS_Mask    (0x7UL << RCC_CFGR1_SWS_Pos)
#define RCC_CFGR1_SWS_HSI     0x1UL
#define RCC_CFGR1_SWS_HSE     0x2UL
#define RCC_CFGR1_SWS_PLL1    0x3UL

#define RCC_CFGR1_SW_Pos      3
#define RCC_CFGR1_SW_Mask     (0x7UL << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SW_HSI      (0x1UL << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SW_HSE      (0x2UL << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SW_PLL1     (0x3UL << RCC_CFGR1_SW_Pos)

/* --- RCC PLL1CFGR --- */
#define RCC_PLL1CFGR_PLL1SRC_Pos   0
#define RCC_PLL1CFGR_PLL1SRC_Mask  (0x3UL << RCC_PLL1CFGR_PLL1SRC_Pos)
#define RCC_PLL1CFGR_PLL1SRC_HSI   (0x1UL << RCC_PLL1CFGR_PLL1SRC_Pos)
#define RCC_PLL1CFGR_PLL1SRC_HSE   (0x2UL << RCC_PLL1CFGR_PLL1SRC_Pos)

#define RCC_PLL1CFGR_PLL1M_Pos     4
#define RCC_PLL1CFGR_PLL1M_Mask    (0x3FUL << RCC_PLL1CFGR_PLL1M_Pos)
#define RCC_PLL1CFGR_PLL1M(v)      ((v) << RCC_PLL1CFGR_PLL1M_Pos)

#define RCC_PLL1CFGR_PLL1RGE_Pos   14
#define RCC_PLL1CFGR_PLL1VCOSEL    (1UL << 17)
#define RCC_PLL1CFGR_PLL1FRACEN    (1UL << 18)

#define RCC_PLL1CFGR_PLL1N_Pos     20
#define RCC_PLL1CFGR_PLL1N_Mask    (0x1FFUL << RCC_PLL1CFGR_PLL1N_Pos)
#define RCC_PLL1CFGR_PLL1N(v)      ((v) << RCC_PLL1CFGR_PLL1N_Pos)

/* --- RCC PLL1DIVR --- */
#define RCC_PLL1DIVR_PLL1R_Pos     0
#define RCC_PLL1DIVR_PLL1R(v)      (((v) - 1) << RCC_PLL1DIVR_PLL1R_Pos)
#define RCC_PLL1DIVR_PLL1Q(v)      (((v) - 1) << 6)
#define RCC_PLL1DIVR_PLL1P(v)      (((v) - 1) << 12)

/* --- RCC AHB2ENR (GPIO clock gates) --- */
#define RCC_AHB2ENR_GPIOAEN        (1UL << 0)
#define RCC_AHB2ENR_GPIOBEN        (1UL << 1)
#define RCC_AHB2ENR_GPIOCEN        (1UL << 2)
#define RCC_AHB2ENR_GPIODEN        (1UL << 3)
#define RCC_AHB2ENR_GPIOEEN        (1UL << 4)
#define RCC_AHB2ENR_GPIOFEN        (1UL << 5)
#define RCC_AHB2ENR_GPIOGEN        (1UL << 6)
#define RCC_AHB2ENR_GPIOHEN        (1UL << 7)

/* --- RCC APB1ENR1 --- */
#define RCC_APB1ENR1_USART2EN      (1UL << 17)
#define RCC_APB1ENR1_USART3EN      (1UL << 18)

/* --- RCC APB2ENR --- */
#define RCC_APB2ENR_USART1EN       (1UL << 14)

/* --- GPIO MODER --- */
#define GPIO_MODER_INPUT           0x0UL
#define GPIO_MODER_OUTPUT          0x1UL
#define GPIO_MODER_AF              0x2UL
#define GPIO_MODER_ANALOG          0x3UL

/* --- GPIO PUPDR --- */
#define GPIO_PUPDR_NONE            0x0UL
#define GPIO_PUPDR_PU              0x1UL
#define GPIO_PUPDR_PD              0x2UL

/* --- GPIO OSPEEDR --- */
#define GPIO_OSPEEDR_LOW           0x0UL
#define GPIO_OSPEEDR_MEDIUM        0x1UL
#define GPIO_OSPEEDR_HIGH          0x2UL
#define GPIO_OSPEEDR_VERY_HIGH     0x3UL

/* --- USART CR1 --- */
#define USART_CR1_UE               (1UL << 0)
#define USART_CR1_RE               (1UL << 2)
#define USART_CR1_TE               (1UL << 3)
#define USART_CR1_RXNEIE           (1UL << 5)
#define USART_CR1_TXEIE            (1UL << 7)
#define USART_CR1_FIFOEN           (1UL << 29)

/* --- USART ISR --- */
#define USART_ISR_TXE              (1UL << 7)
#define USART_ISR_TC               (1UL << 6)
#define USART_ISR_RXNE             (1UL << 5)

/* --- SysTick CTRL --- */
#define SysTick_CTRL_ENABLE        (1UL << 0)
#define SysTick_CTRL_TICKINT       (1UL << 1)
#define SysTick_CTRL_CLKSOURCE     (1UL << 2)
#define SysTick_CTRL_COUNTFLAG     (1UL << 16)

/* --- SCB AIRCR --- */
#define SCB_AIRCR_VECTKEY          (0x05FAUL << 16)
#define SCB_AIRCR_PRIGROUP_Pos     8

/* --- FPU CPACR --- */
#define FPU_CPACR_CP10_FULL        (0x3UL << 20)
#define FPU_CPACR_CP11_FULL        (0x3UL << 22)

/* --- FLASH ACR --- */
#define FLASH_ACR_LATENCY_Pos      0
#define FLASH_ACR_PRFTEN           (1UL << 8)

/* ================================================================
 *  Macros
 * ================================================================ */
#define BIT(n)                     (1UL << (n))

/* ================================================================
 *  Board pin map — NUCLEO-U575ZI-Q
 *  Adjust per your hardware.
 * ================================================================ */
#define LED_PORT                   GPIOC
#define LED_PIN                    7
#define BTN_PORT                   GPIOC
#define BTN_PIN                    13
#define UART_TX_PORT               GPIOA
#define UART_TX_PIN                2
#define UART_RX_PORT               GPIOA
#define UART_RX_PIN                3
#define DEBUG_UART                 USART2

/* ================================================================
 *  Clock values (Hz)
 * ================================================================ */
#define HSI_VALUE                  16000000U
#define HSE_VALUE                  8000000U
#define SYSCLK_VALUE               160000000U

#endif /* STM32U575XX_H */
