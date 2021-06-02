#ifndef __COMM_UART_H__
#define __COMM_UART_H__

int comm_uart_init(void);
uint8_t comm_uart_getchar(void);
int comm_uart_putchar(const rt_uint8_t ch);
int comm_uart_sendstr(const char *str, size_t len);
int comm_uart_deinit(void);

#endif//__COMM_UART_H__/