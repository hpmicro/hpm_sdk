#ifndef UART_DEMO_CONF_H
#define UART_DEMO_CONF_H

int uart_dma_rx_init(void);
size_t get_uart_data(uint8_t* dst_buf, size_t *avilable_len);
#endif /* UART_DEMO_CONF_H */