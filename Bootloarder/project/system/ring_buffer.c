#include "ring_buffer.h"

#define Z_INTERNAL_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MIN(a,b) Z_INTERNAL_MIN(a,b)
/********************************************ring_buf_tx*********************************************/
static inline int ring_buf_tx_area_finish(ring_buffer_tx_t *buf, struct ring_buf_tx_index *ring,uint32_t size)
{
	uint16_t claimed_size, tail_offset;

	claimed_size = ring->head - ring->tail;
	if (size > claimed_size)
	{
		return -1;
	}

	ring->tail += size;
	ring->head = ring->tail;

	tail_offset = ring->tail - ring->base;
	if (tail_offset >= buf->size) 
	{
		ring->base += buf->size;
	}

	return 0;
}

int ring_buf_tx_put_finish(ring_buffer_tx_t *buf, uint32_t size)
{
	return ring_buf_tx_area_finish(buf, &buf->put, size);
}

static inline uint32_t ring_buf_tx_space_get(const ring_buffer_tx_t *buf)
{
	uint16_t allocated = buf->put.head - buf->get.tail;
	return buf->size - allocated;
}

static inline uint32_t ring_buf_tx_size_get(const ring_buffer_tx_t *buf)
{
	uint16_t size = buf->put.tail - buf->get.head;
	return size;
}

uint32_t ring_buf_tx_area_claim(ring_buffer_tx_t *buf, struct ring_buf_tx_index *ring,
			     uint8_t **data, uint32_t size)
{
	uint16_t head_offset,wrap_size;
	head_offset = ring->head - ring->base;
	
	if (head_offset >= buf->size)
	{
		head_offset -= buf->size;
	}
	
	wrap_size = buf->size - head_offset;
	size = MIN(size, wrap_size);
	
	*data = &buf->buffer[head_offset];
	ring->head += size;

	return size;
}

inline uint32_t ring_buf_tx_put_claim(ring_buffer_tx_t *buf,\
																					uint8_t **data,\
																					uint32_t size)
{
	uint32_t space = ring_buf_tx_space_get(buf);
	return  ring_buf_tx_area_claim(buf, &buf->put, data,
				   MIN(size, space));
}

inline uint32_t ring_buf_tx_read_claim(ring_buffer_tx_t *buf,\
																					uint8_t **data,\
																					uint32_t size)
{
	uint32_t ready_readbuf_size = ring_buf_tx_size_get(buf);
	return ring_buf_tx_area_claim(buf, &buf->get, data,
				   MIN(size, ready_readbuf_size));
}

inline int8_t ring_buf_tx_init(ring_buffer_tx_t *ring_buf,\
													  uint32_t size)
{
	ring_buf->size = size;
	int8_t result = memory_allocation(size,&ring_buf->buffer);
	ring_buf->put.head = ring_buf->put.tail	= ring_buf->put.base = 0;
	ring_buf->get.head = ring_buf->get.tail = ring_buf->get.base = 0;
	return result;
}
/********************************************ring_buf_rx*********************************************/
int ring_buf_rx_space(ring_buffer_rx_t *buf)
{
	uint16_t allocated = buf->rx_head - buf->rx_tail;
	return buf->size - allocated;	
}

static inline int ring_buf_rx_area_finish(ring_buffer_rx_t *buf,uint32_t size)
{
	uint16_t head_offset;
	buf->rx_head += size;
	
	head_offset = buf->rx_head - buf->rx_base;
	if(head_offset > buf->size)
	{
		buf->rx_base += buf->size;
	}
	return 0;
}

int ring_buf_rx_put_finish(ring_buffer_rx_t *buf, uint32_t size)
{
	return ring_buf_rx_area_finish(buf, size);
}

inline int8_t ring_buf_rx_init(ring_buffer_rx_t *ring_buf,\
															 uint32_t size)
{
	ring_buf->size = size;
	int8_t result = memory_allocation(size,&ring_buf->buffer);
	ring_buf->rx_head = ring_buf->rx_tail = ring_buf->rx_base = 0;
	return result;
}

