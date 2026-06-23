#include "memory_alloc.h"
#include "at32f435_437.h"

#define memory_pool_size 16 * 1024
uint8_t memory_pool[memory_pool_size];
uint32_t memory_index = 0;

int8_t memory_allocation(uint32_t size,uint8_t **buffer)
{
	if(memory_index + size >= memory_pool_size)
	{
		#ifdef DEBUG
		__BKPT(0);
		#endif
		return -1;
	}
	
	*buffer = &memory_pool[memory_index];
	memory_index += size;
	return 1;
}
