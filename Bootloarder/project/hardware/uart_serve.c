#include "uart_serve.h"
#include "memory_alloc.h"
#include "ring_buffer.h"

#include "wk_dma.h"
#include "string.h"

#define start_bit 1
#define uart_tx_cache_ms 10//最快发送频率 单位/ms

uart_tx_ring_pool_t usart1_tx_ring_pool;

uint8_t USART2_RX_BUF[USART2_RX_BUF_SIZE * 2] __attribute__ ((aligned (4)));
uint8_t USART2_RX_BUF1[USART2_RX_BUF_SIZE] __attribute__ ((aligned (4)));

volatile int _serial_rx_fp, _serial_rx_lp, _serial_rx_len;

uint32_t UART_RingSpaceInit_ByBaudrate(usart_type* usart_x,uart_config_t *uart_set)
{
	uint32_t apb_clock;
	
	crm_clocks_freq_type clocks_freq;
	uart_config_t uart_config;
	
	crm_clocks_freq_get(&clocks_freq);
	
	if(usart_x == USART1 || usart_x == USART6)
		apb_clock = clocks_freq.apb2_freq;
	else 
		apb_clock = clocks_freq.apb1_freq;
	
	uint32_t div = usart_x->baudr_bit.div;
	uart_config.baudrate = apb_clock / div;
	
	if(usart_x->ctrl1_bit.dbn_h == 0 && usart_x->ctrl1_bit.dbn_l == 0)
		uart_config.uart_frame_config.data_bit = 8;	
	else if(usart_x->ctrl1_bit.dbn_h == 0 && usart_x->ctrl1_bit.dbn_l == 1)
		uart_config.uart_frame_config.data_bit = 9;
	else 
		uart_config.uart_frame_config.data_bit = 7;
	
	if(usart_x->ctrl2_bit.stopbn == 0 || usart_x->ctrl2_bit.stopbn == 1)
		uart_config.uart_frame_config.stop_bits = 1;
	else 
		uart_config.uart_frame_config.stop_bits = 2;

	if(usart_x->ctrl1_bit.pen != FALSE)
		uart_config.uart_frame_config.check_bits = 1;
	else 
		uart_config.uart_frame_config.check_bits = 0;
	
	*uart_set = uart_config;
	 uint32_t frame_bits = start_bit +
												uart_config.uart_frame_config.data_bit +
												uart_config.uart_frame_config.check_bits +
												uart_config.uart_frame_config.stop_bits;

	uint32_t bytes_per_sec = uart_config.baudrate / frame_bits;//限制每秒字节数

	uint32_t ring_size = (bytes_per_sec * uart_tx_cache_ms + 999U) / 1000U;//限制每100hz 可以发送的字节数是多少
	
	return ring_size;
}

void serial_receive(int len)
{
	if (len + _serial_rx_len < USART2_RX_BUF_SIZE)
	{
		__set_PRIMASK(1);
		if ((len + _serial_rx_fp) > USART2_RX_BUF_SIZE)
		{
			int _len = (USART2_RX_BUF_SIZE - _serial_rx_fp);
			memcpy(&USART2_RX_BUF[_serial_rx_fp],
					USART2_RX_BUF1,
					_len);
			memcpy(&USART2_RX_BUF[0],
					USART2_RX_BUF1 + _len,
					len - _len);
		}
		else
		{
			memcpy(&USART2_RX_BUF[_serial_rx_fp],
					USART2_RX_BUF1, len);
		}
		_serial_rx_len += len;
		_serial_rx_fp = (_serial_rx_fp + len) % USART2_RX_BUF_SIZE;
		__set_PRIMASK(0);
	}
}

static inline void serial_disable()
{
	/* Disable the Selected IRQ Channels -------------------------------------*/
	__set_PRIMASK(1);
}

static inline void serial_enable()
{
	/* Enable the Selected IRQ Channels --------------------------------------*/
	__set_PRIMASK(0);
}

size_t serial_available(void)
{
	serial_disable();
	int buf = _serial_rx_len;
	serial_enable();
	return buf;
}

int serial_read(void)
{
	if (serial_available() == 0)
	{
		return -1;
	}
	serial_disable();
	uint8_t buf = USART2_RX_BUF[_serial_rx_lp];
	_serial_rx_lp = (_serial_rx_lp + 1) % USART2_RX_BUF_SIZE;
	_serial_rx_len--;
	serial_enable();
	return buf;
}

void UART_Serve_Init()
{
	uint32_t Size = UART_RingSpaceInit_ByBaudrate(USART1,&usart1_tx_ring_pool.uart_config);
	ring_buf_tx_init(&usart1_tx_ring_pool.ring_buffer, Size);
			
	wk_dma_channel_config(DMA1_CHANNEL1, 
                       (uint32_t)&USART1->dt, 
                       (uint32_t)&USART2_RX_BUF1, 
                       USART2_RX_BUF_SIZE);
	
  dma_channel_enable(DMA1_CHANNEL1, TRUE);	
}
