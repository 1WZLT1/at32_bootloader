#ifndef __DECODE_H
#define __DECODE_H
#include "stdint.h"

extern uint8_t CRC8_Table(uint8_t* p, uint8_t counter);
extern uint16_t CRC16_Table(uint8_t* p, uint8_t counter);
extern uint32_t CRC32_Table(uint8_t* buf, int len);
extern uint32_t CRC32_Table_Without(uint8_t* buf, int len,uint32_t* without);

extern void Decode(const uint8_t* table,uint8_t* values_in,uint8_t* values_out,int len);
extern void Decode112(const uint8_t* table,uint8_t* values_in,uint8_t* values_out,int len);
extern void DecodeMessage(uint8_t* values_in,uint8_t* values_out,int len);
extern void DecodeMCU(uint8_t* values_in,uint8_t* values_out,int len);
uint32_t CRC32_Update(uint32_t crc_state, uint8_t* buf, int len);
uint32_t CRC32_Final(uint32_t crc_state);
#endif
