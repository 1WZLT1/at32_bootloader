#include "SerialBoot.h"
#include "Decode.h"
#include "wk_dma.h"
#include "at32f435_437.h"
#include "at32f435_437_int.h"
#include "Hardware_Name.h"
#include "at32f435_437_flash.h"
#include "IAP.h"

static int BootStatus = 0;
static int ReceiveStatus = 0;

static int ReceiveDataLeft = 0;
static int ReceiveType = 0;
static uint32_t BufferIndex;
static uint8_t ReceiveTxNextNumber = 0;
static uint8_t SerialBoot_Frame_Buffer[10];
static uint8_t WaterNumber = 0;
static uint8_t SerialBoot_CRC8Verify;
static uint16_t SerialBoot_CRC16Verify;

#define BufferSize 2048
static uint16_t Bufferx16[BufferSize / 2];
static uint8_t* const Buffer = (uint8_t*)Bufferx16;

void SerialBoot_Reset()
{
	ReceiveStatus = SerialBoot_Frame_Start;
	ReceiveDataLeft = 0;
	ReceiveType = 0;
	BufferIndex = 0;
	ReceiveTxNextNumber = 0;
}

static inline void ResetMCU()
{
	NVIC_SystemReset();
}

void SerialBoot_Error()
{
	ReceiveStatus = SerialBoot_Frame_Start;
}

void SerialBoot_Insert(uint8_t value)
{
	if (ReceiveDataLeft <= 0)
	{
		SerialBoot_Error();
		return;
	}
	if (ReceiveStatus != SerialBoot_Frame_Data)
	{
		SerialBoot_Error();
		return;
	}
	Buffer[BufferIndex++] = value;  //将元素放入队列尾部
	if (BufferIndex >= BufferSize)
	{
		SerialBoot_Error();
		return;
	}
	ReceiveDataLeft--;
}

int SerialBoot_RunningData(uint8_t value)
{
	if (ReceiveStatus < SerialBoot_Frame_Start || ReceiveStatus > SerialBoot_Frame_End)
	{
		__BKPT(3);
		//SerialBoot_Error();
		return -3;
	}
	SerialBoot_Frame_Buffer[ReceiveStatus] = value;
	switch (ReceiveStatus)
	{
		case SerialBoot_Frame_Start:
		{
			SerialBoot_Reset();
			if (value == SerialBoot_Connect_Flag)
			{
				return 0;
			}
			if (value != SerialBoot_STX_Flag)
			{
				SerialBoot_Error();
				return -1;
			}
			ReceiveStatus = SerialBoot_Frame_CMD;
			break;
		}
		case SerialBoot_Frame_CMD:
		{
			ReceiveType = value;
			ReceiveStatus = SerialBoot_Frame_Length;
			break;
		}
		case SerialBoot_Frame_Length:
		{
			ReceiveDataLeft = value;
			ReceiveStatus = SerialBoot_Frame_SerialNumber;
			break;
		}
		case SerialBoot_Frame_SerialNumber:
		{
			WaterNumber = value;
			ReceiveStatus = SerialBoot_Frame_CRC8;
			break;
		}
		case SerialBoot_Frame_CRC8:
		{
			SerialBoot_CRC8Verify = value;
			if (CRC8_Table(SerialBoot_Frame_Buffer, SerialBoot_Frame_CRC8) != SerialBoot_CRC8Verify)
			{
				SerialBoot_Error();
				return -1;
			}
			if(ReceiveDataLeft == 0)
			{
				ReceiveStatus = SerialBoot_Frame_Start;
				return 1;
			}
			else
			{
				ReceiveStatus = SerialBoot_Frame_CRC16H;
				return 0;
			}
		}
		case SerialBoot_Frame_CRC16H:
		{
			SerialBoot_CRC16Verify = value;
			ReceiveStatus = SerialBoot_Frame_CRC16L;
			break;
		}
		case SerialBoot_Frame_CRC16L:
		{
			SerialBoot_CRC16Verify = (SerialBoot_CRC16Verify << 8) | value;
			ReceiveStatus = SerialBoot_Frame_Data;
			break;
		}
		case SerialBoot_Frame_Data:
		{
			if (ReceiveDataLeft)
			{
				SerialBoot_Insert(value);
				if (ReceiveDataLeft == 0)
				{
					ReceiveStatus = SerialBoot_Frame_End;
				}
				break;
			}
			else
			{
				ReceiveStatus = SerialBoot_Frame_End;
			}
		}
		case SerialBoot_Frame_End:
		{
			if (value != SerialBoot_EDX_Flag)
			{
				SerialBoot_Error();
				return -1;
			}
			uint16_t CRC16 = CRC16_Table(Buffer, BufferIndex);
			if (CRC16 != SerialBoot_CRC16Verify)
			{
				SerialBoot_Error();
				return -1;
			}
			ReceiveStatus = SerialBoot_Frame_Start;
			return 1;
		}
		default:
		{
			SerialBoot_Error();
			return -1;
		}
	}
	return 0;
}



uint8_t usart1_tx_dma_busy = 0U;
uint8_t usart1_tx_byte;

void SerialWrite(uint8_t data)
{
    while (usart1_tx_dma_busy != 0U)
    {
    }

    usart1_tx_byte = data;

    dma_channel_enable(DMA1_CHANNEL2, FALSE);

    dma_flag_clear(DMA1_FDT2_FLAG);
    dma_flag_clear(DMA1_HDT2_FLAG);

    wk_dma_channel_config(
        DMA1_CHANNEL2,
        (uint32_t)&USART1->dt,
        (uint32_t)&usart1_tx_byte,
        1U);

    usart1_tx_dma_busy = 1U;

    dma_channel_enable(DMA1_CHANNEL2, TRUE);

    while (usart1_tx_dma_busy != 0U){}
    while (USART1->sts_bit.tdc == 0U){}
}

void SerialBoot_Send(uint8_t type, uint8_t* buf, int len)
{
	uint8_t head[5];

	head[0] = SerialBoot_STX_Flag;//0xfc
	head[1] = type;//0x21
	head[2] = len;//2
	head[3] = ReceiveTxNextNumber++;//0x01
	uint8_t CRC8 = CRC8_Table(head, 4);//0xcd
	uint16_t CRC16 = CRC16_Table(buf, len);//0x3063
	for (int i = 0; i < 4; i++)
	{
		SerialWrite(head[i]);
	}
	SerialWrite(CRC8);
	if(len > 0)
	{
		SerialWrite(CRC16 >> 8);
		SerialWrite(CRC16 & 0xff);
		for (int i = 0; i < len; i++)
		{
			SerialWrite(buf[i]);
		}
		SerialWrite(SerialBoot_EDX_Flag);
	}
}

void SerialBoot_SendOver()
{
	uint8_t buf[2];
	buf[0] = WaterNumber;
	buf[1] = SerialBoot_StatusType_Idle;
	SerialBoot_Send(SerialBoot_FrameCMD_Status, buf, 2);
}

static void SerialBoot_FirmwareName(const Firmware_Data_t* Firmware,uint32_t ID)
{
	static uint32_t buffer[64];
	static SerialBoot_Firmware_Name_t* const list = (SerialBoot_Firmware_Name_t*)buffer;
	int index = Firmware->Firmware_Name;
	uint32_t block = Firmware->Firmware_Block;
	const uint8_t* buf = (const uint8_t*)Firmware;
	int len = buf[index++];
	if(block > 2)
	{
		return;
	}
	list->ID = ID;
	for(int i = 0;i < len;i++)
	{
		list->Name_UTF8[i] = buf[index++];
	}
	SerialBoot_Send(SerialBoot_FrameCMD_Version, (uint8_t*)buffer, len + 4);
}

#define Firmware0		((Firmware_Data_t*)(Firmware0_Base))
static void SerialBoot_Name()
{
	static uint32_t buffer[64];
	static SerialBoot_Firmware_Name_t* const list = (SerialBoot_Firmware_Name_t*)buffer;
	list->ID = 10;
	int len = Get_Hardware_Name(&list->Name_UTF8[0]);
	SerialBoot_Send(SerialBoot_FrameCMD_Version, (uint8_t*)buffer, len + 4);
	int Firmware0_Check = 0;
	Firmware0_Check = !Firmware_Check(Firmware0, 0);
	
	if(Firmware0_Check)
	{
		SerialBoot_FirmwareName(Firmware0, 11);
		SerialBoot_FirmwareName(Firmware0, 14);
	}
}

uint32_t Chip_ID[4];
#define HW_ID_IMU_VF435_V1_688_1111  0x0001010000

#define Firmware0		((Firmware_Data_t*)(Firmware0_Base))

void SerialBoot_Version()
{
	SerialBoot_Name();
	SerialBoot_Version_t SerialBoot_Version_Buffer;
	uint32_t flash = FDI_Get_ChipID_128(0,Chip_ID);
	
	SerialBoot_Version_Buffer.ID = 2;
	SerialBoot_Version_Buffer.Hardware_Version = BOARD_ID;
	SerialBoot_Version_Buffer.Chip_ID = (Chip_ID[0] + Chip_ID[1]) ^ Chip_ID[2];
	SerialBoot_Version_Buffer.Full_ID[0] = Chip_ID[0];
	SerialBoot_Version_Buffer.Full_ID[1] = Chip_ID[1];
	SerialBoot_Version_Buffer.Full_ID[2] = Chip_ID[2];
	SerialBoot_Version_Buffer.Full_ID[3] = Chip_ID[3];
	SerialBoot_Version_Buffer.Firmware_Entry = BOOT_BASE;
	SerialBoot_Version_Buffer.Boot_Address = 0x08000000;
	SerialBoot_Version_Buffer.Boot_Length = BOOT_BASE - 0x08000000;
	
	Firmware_Info_t* _info = SerialBoot_Version_Buffer.Firmware;
	#ifdef Firmware1_Base
	_info->Address = Firmware0_Base;
	_info->Length = Firmware1_Base - Firmware0_Base;
	_info->Checked = Firmware_Check(Firmware0, 0);
	_info->Version = _info->Checked == 0 ? Firmware0->Firmware_Version : 0;
	_info++;
	_info->Address = Firmware1_Base;
	_info->Length = 2 * Firmware1_Base - Firmware0_Base;
	_info->Checked = Firmware_Check(Firmware1, 1);
	_info->Version = _info->Checked == 0 ? Firmware1->Firmware_Version : 0;
	_info++;
	#else
	_info->Address = Firmware0_Base;
	_info->Length = (flash + 0x08000000) - Firmware0_Base;
	_info->Checked = 0;
	_info->Version = _info->Checked == 0 ? Firmware0->Firmware_Version : 0;
	_info++;
	_info->Address = 0;
	_info->Length = 0;
	_info->Checked = 0;
	_info->Version = 0;
	_info++;
	#endif
//	_info->Address = Firmware2_Base;
//	_info->Length = 2 * Firmware2_Base - Firmware1_Base;
//	_info->Checked = Firmware_Check(Firmware2, 2);
//	_info->Version = Firmware2->Firmware_Version;
	_info->Address = 0;
	_info->Length = 0;
	_info->Checked = 0;
	_info->Version = 0;

	SerialBoot_Send(SerialBoot_FrameCMD_Version, (uint8_t*)&SerialBoot_Version_Buffer, sizeof(SerialBoot_Version_t));
}

void SerialBoot_Unlock()
{
	flash_unlock();
}

#define APP_FLASH_START      ((uint32_t)0x08010000U)
#define APP_FLASH_SIZE       ((uint32_t)0x00060000U)
#define APP_FLASH_END        (APP_FLASH_START + APP_FLASH_SIZE)  // 0x08070000
#define FLASH_BLOCK_SIZE     ((uint32_t)0x00010000U)             // 64KB

flash_status_type SerialBoot_Erasure()
{
		flash_status_type status;
    uint32_t address;
	
    flash_flag_clear(FLASH_BANK1_ODF_FLAG     |
                     FLASH_BANK1_PRGMERR_FLAG |
                     FLASH_BANK1_EPPERR_FLAG);

    for(address = APP_FLASH_START;
        address < APP_FLASH_END;
        address += FLASH_BLOCK_SIZE)
    {
        status = flash_block_erase(address);

        if(status != FLASH_OPERATE_DONE)
        {
            flash_lock();
            return status;
        }
    }
		return status;
}

void FDI_Flash_Program(void* address, const uint8_t* buffer, int length)
{
	uint32_t addr = (uint32_t) address;
	uint32_t buf = (uint32_t)buffer;
	if (addr < 0x08000000)
	{
		__BKPT(3);
	}
	if(length <= 0)
	{
		__BKPT(3);
	}
	while (length > 0)
	{
		flash_byte_program(addr, *(uint8_t*)buf);
		buf += 1;
		addr += 1;
		length -= 1;
	}	
}

void SerialBoot_Download()
{
	static uint8_t buffer[112];
	if (BufferIndex != 8 + 112)
	{
		SerialBoot_Error();
		return;
	}
	//Firmware1_Check = 0;
	uint32_t Addr =
		((((Buffer[0]) * 0x100 + Buffer[1]) * 0x100 + Buffer[2]) * 0x100 + Buffer[3]);
	int Len = Buffer[4];
	DecodeMCU(&Buffer[8], buffer, 112);	
	
	FDI_Flash_Program((void*)Addr,buffer,Len);
}

void SerialBoot_Download_Message()
{
	static uint8_t buffer[112];
	if (BufferIndex != 8 + 112)
	{
		SerialBoot_Error();
		return;
	}
	
	uint32_t Addr =
		((((Buffer[0]) * 0x100 + Buffer[1]) * 0x100 + Buffer[2]) * 0x100 + Buffer[3]);
	int Len = Buffer[4];
	DecodeMessage(&Buffer[8], buffer, 112);

	FDI_Flash_Program((void*)Addr, buffer, Len);
}

int SerialBoot_CheckData(uint8_t len)
{
	if (BufferIndex != len)
	{
		SerialBoot_Error();
		return -1;
	}
	return 0;
}

void SerialBoot_MCUReset()
{
	if (SerialBoot_CheckData(0))
	{
		return;
	}
	
	uint8_t buf[2];
	buf[0] = WaterNumber;
	buf[1] = SerialBoot_StatusType_Break;
	SerialBoot_Send(SerialBoot_FrameCMD_Status, buf, 2);
	
	flash_lock();
	ResetMCU();
}

int ReceiveCount[256] = {0};
void SerialBoot_Effect()
{
	static int lastWaterNumber = 0xff;
	static int64_t lastTime = 0;
	int64_t dt = Micros() - lastTime;
	lastTime += dt;
	if(dt < 5000000 && lastWaterNumber == WaterNumber)
	{
		SerialBoot_SendOver();
		return;
	}
	lastWaterNumber = WaterNumber;
	ReceiveCount[ReceiveType]++;
	switch (ReceiveType)
	{
		case SerialBoot_FrameCMD_Version:
			SerialBoot_Version();
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Keep:
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Unlock:
			SerialBoot_Unlock();
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Erasure:
			SerialBoot_Erasure();
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Download:
			SerialBoot_Download();
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Download_Message:
			SerialBoot_Download_Message();
			SerialBoot_SendOver();
			break;
		case SerialBoot_FrameCMD_Reset:
			SerialBoot_MCUReset();
			break;
	}
}

int SerialBoot_UnconnectCount = 0;
int SerialBoot_Initialize()
{
	SerialBoot_UnconnectCount = 0;
	//LastTime = HAL_Clock_Get_us();
	BufferIndex = 0;
	BootStatus = SerialBoot_Status_Unconnected;
	ReceiveStatus = SerialBoot_Frame_Start;
	for (int i = 0; i < BufferSize; i++)
	{
		Buffer[i] = 0;
	}
	return 0;

}

int SerialBoot_Receive(uint8_t value)
{
	if (BootStatus == SerialBoot_Status_Running)
	{
		uint8_t result = SerialBoot_RunningData(value);
		if (result == 1)
		{
			SerialBoot_Effect();
		}
		return result;
	}
	else if (BootStatus == SerialBoot_Status_Unconnected)
	{
		if (value == SerialBoot_Connect_Flag)
		{
			SerialBoot_UnconnectCount++;
			if (SerialBoot_UnconnectCount >= 5)
			{
				BootStatus = SerialBoot_Status_Running;
				SerialBoot_SendOver();		//当前
			}
			uint8_t result = SerialBoot_RunningData(value);
			if (result == 1)
			{
				SerialBoot_Effect();
				BootStatus = SerialBoot_Status_Running;
			}
			return SerialBoot_Return_OK;
		}
		else
		{
			return SerialBoot_Error_InvalidStart;
		}
	}
	else
	{
		return SerialBoot_Error_InvalidStatus;
	}
}
