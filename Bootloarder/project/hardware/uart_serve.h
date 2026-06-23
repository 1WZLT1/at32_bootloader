#ifndef __uart_serve_h
#define __uart_serve_h

#include <stdio.h>
#include <stdint.h>
#include "ring_buffer.h"
#include "at32f435_437_usart.h"

#define USART2_RX_BUF_SIZE 4096

typedef struct 
{
	uint8_t data_bit  : 4;
	uint8_t stop_bits : 2;
	uint8_t check_bits: 2;
}uart_frame_config_t;

typedef struct
{
	uint32_t   baudrate;
	uart_frame_config_t uart_frame_config;
	
}uart_config_t;

typedef struct
{
	ring_buffer_tx_t ring_buffer;
	uart_config_t uart_config;
}uart_tx_ring_pool_t;

typedef struct
{
	ring_buffer_rx_t ring_buffer;
	uart_config_t uart_config;
}uart_rx_ring_pool_t;

uint32_t UART_RingSpaceInit_ByBaudrate(usart_type* usart_x,uart_config_t *uart_set);
void UART_Serve_Init(void);
void serial_receive(int len);
size_t serial_available(void);
int serial_read(void);

extern uart_tx_ring_pool_t usart1_tx_ring_pool;
extern uart_rx_ring_pool_t usart1_rx_ring_pool;
extern uint8_t USART2_RX_BUF[USART2_RX_BUF_SIZE * 2];
extern uint8_t USART2_RX_BUF1[USART2_RX_BUF_SIZE];
extern volatile int _serial_rx_len;
#endif
