#ifndef __Hardware_Name_h
#define __Hardware_Name_h

#include "stdint.h"

int Get_Hardware_Name(uint8_t *buffer);
uint32_t FDI_Get_ChipID_128(uint16_t product, uint32_t *buf);

__packed typedef struct Firmware_Data
{
	/********************************************************
	 * 中断向量表
	 * 长度:1KBytes
	 ********************************************************/
	uint32_t	vector[256];						//为兼容MCU中断向量表

	/********************************************************
	 * 固件信息
	 * 长度:256Bytes
	 ********************************************************/
	uint32_t	Info_Version;						//1:Info_Version 的版本号,用于做向后兼容处理
	uint32_t	Info_Verify;					    //2:整个表除这个元素以外的CRC32 多项式:0xEDB88320
	uint32_t	Hardware_Version;                   //3:硬件ID(版本号)
	uint32_t	Hardware_Name;                      //4:硬件名称
	uint32_t	Firmware_Block;                     //5:判断固件是属于原厂区/固件1区/固件2区
	uint32_t	Firmware_Address;                   //6:固件起始地址,这个地址是指从固件信息段之后的段地址
	uint32_t	Firmware_Size;                      //7:固件该段的大小
	uint32_t	Firmware_Version;                   //8:固件ID(版本号)
	uint32_t	Firmware_Name;                      //9:固件名称
	uint32_t	Firmware_Verify;                    //10:固件CRC32校验 多项式:0xEDB88320
	uint32_t    Reserve[64 - 10];

	/********************************************************
	 * Strings信息,包含所有字符串
	 * 长度:256Bytes+动态
	 ********************************************************/
	uint32_t	Strings_Size;						//
	uint32_t	Strings_List[64 * 3 - 1];
} Firmware_Data_t;

#endif

