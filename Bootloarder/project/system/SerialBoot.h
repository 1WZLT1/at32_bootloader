#ifndef __SERIAL_BOOT_H
#define __SERIAL_BOOT_H
#include <stdint.h>

//未初始化
#define SerialBoot_Status_Uninitialized 	0
//上位机无连接
#define SerialBoot_Status_Unconnected 		1
//运行
#define SerialBoot_Status_Running 			  2
//正忙
#define SerialBoot_Status_Busy 				    3

#define SerialBoot_Frame_Start				0
#define SerialBoot_Frame_CMD				  1
#define SerialBoot_Frame_Length				2
#define SerialBoot_Frame_SerialNumber 3
#define SerialBoot_Frame_CRC8				  4
#define SerialBoot_Frame_CRC16H				5
#define SerialBoot_Frame_CRC16L				6
#define SerialBoot_Frame_Data				  7
#define SerialBoot_Frame_End				  8

/***************************************************************
 *	下行指令
 **************************************************************/
#define SerialBoot_FrameCMD_Version			0x0c	//	获得版本
#define SerialBoot_FrameCMD_Keep 			  0x0d	//	保持连接
#define SerialBoot_FrameCMD_Unlock 			0x0e	//	解锁
#define SerialBoot_FrameCMD_Erasure 		0x0f	//	擦除
#define SerialBoot_FrameCMD_Download 		0x10	//	下载
#define SerialBoot_FrameCMD_Reset 			0x13	//	跳转
#define SerialBoot_FrameCMD_Firmware		0x14	//	获得固件信息
#define SerialBoot_FrameCMD_Download_Message 	0x16	//	下载

/***************************************************************
 *	上行指令
 **************************************************************/
#define SerialBoot_FrameCMD_Status 			0x21	//	当前状态

#define SerialBoot_StatusType_Idle			0x03
#define SerialBoot_StatusType_Break			0x04

#define SerialBoot_Return_OK				0
#define SerialBoot_Error_InvalidStart		-10	//无效的Start符
#define SerialBoot_Error_InvalidStatus		-11	//无效的状态
#define SerialBoot_Error_CRC8				-12	//错误的CRC8
#define SerialBoot_Error_CRC16				-13	//错误的CRC16

/***************************************************************
*	帧符号
***************************************************************/

#define SerialBoot_STX_Flag 0xFC
#define SerialBoot_EDX_Flag 0xFD
#define SerialBoot_Connect_Flag 0xFD

__packed typedef struct
{
	uint32_t 		ID;							//ID = 10 -> Hardware 11 -> Firmware0 12 -> Firmware1 13 -> Firmware2
	uint8_t			Name_UTF8[64];
}__attribute__((packed)) SerialBoot_Firmware_Name_t;

__packed typedef struct{
	uint32_t		Address;
	uint32_t		Length;
	uint32_t		Checked;
	uint32_t		Version;
}Firmware_Info_t;

__packed typedef struct
{
	uint32_t 		ID;							//ID = 2
	uint32_t		Hardware_Version;			//硬件ID
	uint32_t		Boot_Address;				//单位字节
	uint32_t		Boot_Length;				//单位字节
	uint32_t		Firmware_Entry;					//应用程序入口
	uint32_t		Chip_ID;					//芯片缩略ID
	uint32_t		Full_ID[4];
	Firmware_Info_t	Firmware[3];				//固件区
}__attribute__((packed)) SerialBoot_Version_t;

int SerialBoot_Initialize(void);
int SerialBoot_Receive(uint8_t value);
extern uint8_t usart1_tx_dma_busy;
#endif
