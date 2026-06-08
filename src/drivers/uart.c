#include "uart.h"
#include <stdarg.h>

/* ----------------------------------------------------------------
 *  Minimal itoa / utoa for uart_printf
 * ---------------------------------------------------------------- */
static void reverse(char *s, int len) {
    int i = 0, j = len - 1;
    while (i < j) {
        char t = s[i]; s[i] = s[j]; s[j] = t;
        i++; j--;
    }
}

static int itoa_dec(int n, char *buf) {
    int i = 0;
    unsigned int un = (n < 0) ? (unsigned int)(-n) : (unsigned int)n;
    if (n < 0) buf[i++] = '-';
    int start = i;
    do { buf[i++] = '0' + (un % 10); un /= 10; } while (un);
    reverse(buf + start, i - start);
    buf[i] = '\0';
    return i;
}

static int utoa_hex(unsigned int n, char *buf, int upper) {
    int i = 0;
    char base = upper ? 'A' : 'a';
    do {
        unsigned int d = n & 0xF;
        buf[i++] = (d < 10) ? ('0' + d) : (base + d - 10);
        n >>= 4;
    } while (n);
    buf[i++] = upper ? 'X' : 'x';
    buf[i++] = '0';
    reverse(buf, i);
    buf[i] = '\0';
    return i;
}

/* ----------------------------------------------------------------
 *  uart_init — configure USART2 at `baudrate` bps (8N1)
 *  Assumes GPIO AF is already configured by gpio_init().
 * ---------------------------------------------------------------- */
void uart_init(uint32_t baudrate) {
    /* Enable USART2 clock (APB1, bit 17) */
    volatile uint32_t *apb1enr1 = (volatile uint32_t *)(RCC_BASE + RCC_APB1ENR1_OFFSET);
    *apb1enr1 |= RCC_APB1ENR1_USART2EN;
    (void)*apb1enr1;

    /* Baud rate: USARTDIV = APB_CLK / baudrate (16× oversampling) */
    uint32_t brr = (SystemCoreClock + baudrate / 2U) / baudrate;

    DEBUG_UART->CR1 = 0;
    DEBUG_UART->CR2 = 0;
    DEBUG_UART->CR3 = 0;
    DEBUG_UART->BRR = brr;
    DEBUG_UART->CR1 = USART_CR1_TE | USART_CR1_RE;
    DEBUG_UART->CR1 |= USART_CR1_UE;
}

/* ----------------------------------------------------------------
 *  Blocking TX
 * ---------------------------------------------------------------- */
void uart_putc(char c) {
    while (!(DEBUG_UART->ISR & USART_ISR_TXE)) {}
    DEBUG_UART->TDR = (uint8_t)c;
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

/* ----------------------------------------------------------------
 *  Minimal printf over UART (supports %s %c %d %x %X)
 * ---------------------------------------------------------------- */
void uart_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[32];

    while (*fmt) {
        if (*fmt != '%') { uart_putc(*fmt++); continue; }
        fmt++;
        switch (*fmt) {
        case 's': uart_puts(va_arg(args, const char *)); break;
        case 'c': uart_putc((char)va_arg(args, int)); break;
        case 'd': itoa_dec(va_arg(args, int), buf); uart_puts(buf); break;
        case 'x': utoa_hex(va_arg(args, unsigned int), buf, 0); uart_puts(buf); break;
        case 'X': utoa_hex(va_arg(args, unsigned int), buf, 1); uart_puts(buf); break;
        case '%': uart_putc('%'); break;
        default:  uart_putc('%'); uart_putc(*fmt); break;
        }
        fmt++;
    }
    va_end(args);
}
