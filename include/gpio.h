#ifndef GPIO_H
#define GPIO_H

#include "stm32u575xx.h"

void gpio_init(void);
void gpio_led_on(void);
void gpio_led_off(void);
void gpio_led_toggle(void);
uint8_t gpio_btn_read(void);
void gpio_delay_ms(uint32_t ms);

#endif
