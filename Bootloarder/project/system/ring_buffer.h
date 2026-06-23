#ifndef __ring_buffer_h
#define __ring_buffer_h
	
#include "stdint.h"
#include "memory_alloc.h"

/********************************************ring_buf_tx*********************************************/
struct ring_buf_tx_index { uint16_t head, tail, base; };

typedef struct
{
	uint32_t size;
	uint8_t *buffer;
	
	struct ring_buf_tx_index put;
	struct ring_buf_tx_index get;
} ring_buffer_tx_t;


int8_t ring_buf_tx_init(ring_buffer_tx_t *ring_buf,\
											  uint32_t size);

uint32_t ring_buf_tx_put_claim(ring_buffer_tx_t *buf,\
														   uint8_t **data,\
														   uint32_t size);

uint32_t ring_buf_tx_read_claim(ring_buffer_tx_t *buf,\
														    uint8_t **data,\
														    uint32_t size);

int ring_buf_tx_put_finish(ring_buffer_tx_t *buf, uint32_t size);
/********************************************ring_buf_rx*********************************************/
typedef struct
{
	uint32_t size;
	uint8_t *buffer;
	
	uint16_t rx_head;
	uint16_t rx_tail;
	uint16_t rx_base;
	
	uint8_t  dir;
} ring_buffer_rx_t;

int8_t ring_buf_rx_init(ring_buffer_rx_t *ring_buf,\
															 uint32_t size);

int ring_buf_rx_space(ring_buffer_rx_t *buf);
#endif
