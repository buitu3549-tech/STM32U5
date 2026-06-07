#ifndef UART_H
#define UART_H

#include "stm32u575xx.h"

void uart_init(uint32_t baudrate);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

#endif
