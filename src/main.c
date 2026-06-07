#include "stm32u575xx.h"
#include "system_stm32u5xx.h"
#include "gpio.h"
#include "uart.h"

int main(void) {
    SystemClock_PLL_160M();
    gpio_init();
    uart_init(115200);

    uart_puts("\r\n================================\r\n");
    uart_puts("  STM32U5 Ready\r\n");
    uart_printf("  SYSCLK = %d Hz\r\n", (int)SystemCoreClock);
    uart_puts("================================\r\n\r\n");

    while (1) {
        gpio_led_toggle();
        uart_puts("Heartbeat\r\n");
        gpio_delay_ms(500);
    }
}
