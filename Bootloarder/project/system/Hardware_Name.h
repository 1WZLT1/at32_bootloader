#ifndef __Hardware_Name_h
#define __Hardware_Name_h

#include "stdint.h"

int Get_Hardware_Name(uint8_t *buffer);
uint32_t FDI_Get_ChipID_128(uint16_t product, uint32_t *buf);

#endif

