#include "gpio.h"

static volatile uint32_t systick_ms = 0;

void SysTick_Handler(void) {
    systick_ms++;
}

void gpio_delay_ms(uint32_t ms) {
    uint32_t load = (SystemCoreClock / 1000U) - 1U;
    if (load > 0x00FFFFFFU) load = 0x00FFFFFFU;

    SysTick->LOAD = load;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE
                  | SysTick_CTRL_TICKINT
                  | SysTick_CTRL_ENABLE;

    uint32_t target = systick_ms + ms;
    while (systick_ms < target) {
        __WFI();
    }

    SysTick->CTRL = 0;
}

void gpio_led_on(void) {
    LED_PORT->BSRR = BIT(LED_PIN);
}

void gpio_led_off(void) {
    LED_PORT->BRR = BIT(LED_PIN);
}

void gpio_led_toggle(void) {
    if (LED_PORT->ODR & BIT(LED_PIN))
        gpio_led_off();
    else
        gpio_led_on();
}

uint8_t gpio_btn_read(void) {
    return (BTN_PORT->IDR & BIT(BTN_PIN)) ? 1 : 0;
}

/* ================================================================
 *  gpio_init — configure LED (PC7), Button (PC13), UART2 (PA2/PA3)
 *  GPIO clock is on AHB2 in STM32U5.
 * ================================================================ */
void gpio_init(void) {
    volatile uint32_t *ahb2enr = rcc_reg(RCC_AHB2ENR_OFFSET);

    /* Clock gates */
    *ahb2enr |= RCC_AHB2ENR_GPIOAEN
             |  RCC_AHB2ENR_GPIOBEN
             |  RCC_AHB2ENR_GPIOCEN;
    (void)*ahb2enr;

    /* --- LED (PC7): push-pull output, low speed, no pull --- */
    LED_PORT->MODER   &= ~(0x3UL << (LED_PIN * 2));
    LED_PORT->MODER   |=  (GPIO_MODER_OUTPUT << (LED_PIN * 2));
    LED_PORT->OTYPER  &= ~BIT(LED_PIN);
    LED_PORT->OSPEEDR &= ~(0x3UL << (LED_PIN * 2));
    LED_PORT->PUPDR   &= ~(0x3UL << (LED_PIN * 2));
    gpio_led_off();

    /* --- Button (PC13): input, pull-up --- */
    BTN_PORT->MODER &= ~(0x3UL << (BTN_PIN * 2));
    BTN_PORT->PUPDR &= ~(0x3UL << (BTN_PIN * 2));
    BTN_PORT->PUPDR |=  (GPIO_PUPDR_PU << (BTN_PIN * 2));

    /* --- UART TX (PA2): AF, push-pull, high speed --- */
    UART_TX_PORT->MODER   &= ~(0x3UL << (UART_TX_PIN * 2));
    UART_TX_PORT->MODER   |=  (GPIO_MODER_AF << (UART_TX_PIN * 2));
    UART_TX_PORT->OTYPER  &= ~BIT(UART_TX_PIN);
    UART_TX_PORT->OSPEEDR |=  (GPIO_OSPEEDR_HIGH << (UART_TX_PIN * 2));
    UART_TX_PORT->PUPDR   &= ~(0x3UL << (UART_TX_PIN * 2));
    UART_TX_PORT->AFR[UART_TX_PIN / 8] &= ~(0xFUL << ((UART_TX_PIN % 8) * 4));
    UART_TX_PORT->AFR[UART_TX_PIN / 8] |=  (7UL << ((UART_TX_PIN % 8) * 4));

    /* --- UART RX (PA3): AF, pull-up --- */
    UART_RX_PORT->MODER   &= ~(0x3UL << (UART_RX_PIN * 2));
    UART_RX_PORT->MODER   |=  (GPIO_MODER_AF << (UART_RX_PIN * 2));
    UART_RX_PORT->PUPDR   &= ~(0x3UL << (UART_RX_PIN * 2));
    UART_RX_PORT->PUPDR   |=  (GPIO_PUPDR_PU << (UART_RX_PIN * 2));
    UART_RX_PORT->AFR[UART_RX_PIN / 8] &= ~(0xFUL << ((UART_RX_PIN % 8) * 4));
    UART_RX_PORT->AFR[UART_RX_PIN / 8] |=  (7UL << ((UART_RX_PIN % 8) * 4));
}
