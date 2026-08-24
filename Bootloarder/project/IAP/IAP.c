#include "IAP.h"
#include "SerialBoot.h"
#include "uart_serve.h"
#include "at32f435_437_int.h"
#include "Decode.h"
#include "at32f435_437_ertc.h"

static inline void ResetMCU()
{
	NVIC_SystemReset();
}

#define HW_ID_IMU_VF435_V1_688_1111  0x0001010000
int Firmware_Check(Firmware_Data_t* Firmware, int block)
{
	if(sizeof(Firmware_Data_t) != 2048)
	{
		__BKPT(0);
	}
	if(Firmware->Hardware_Version == 0xffffffff)
	{
		return 18;
	}
	uint32_t crc32 = Firmware->Info_Verify;
	uint32_t new_crc32 = CRC32_Table_Without((uint8_t*)Firmware, sizeof(Firmware_Data_t), (uint32_t *)&Firmware->Info_Verify);
	if (crc32 != new_crc32)
	{
		return 14;
	}
	if (Firmware->Hardware_Version != BOARD_ID)
	{
		return 12;
	}
	crc32 = Firmware->Firmware_Verify;
	uint32_t start = Firmware->Firmware_Address + 0x800;
	uint32_t length = Firmware->Firmware_Size;
	new_crc32 =  CRC32_Table((uint8_t*)start, length);
	if (crc32 != new_crc32)
	{
		return 10;
	}
	
	if(Firmware->Info_Version != 0)
	{
		return 20;
	}
	
	if(Firmware->Firmware_Address != (uint32_t)Firmware)
	{
		return 16;
	}
	if(block >= 0)
	{
		if(Firmware->Firmware_Block != block)
		{
			return 22;
		}
	}
	return 0;
}

int serial_check(void)
{
	__set_PRIMASK(1);
	int buf = _serial_rx_len;
	int len = USART2_RX_BUF_SIZE - DMA1_CHANNEL1->dtcnt;
	__set_PRIMASK(0);
	return (buf > 0) || (len > 0);
}

#define BOOT_STATUS_MAGIC  0x12345678UL
void SetBootStatus(BootStatus_t status)
{
    if (ertc_bpr_data_read(ERTC_DT1) == BOOT_STATUS_MAGIC)
    {
        ertc_bpr_data_write(ERTC_DT2, (uint32_t)status);
    }
    else
    {
        ertc_bpr_data_write(ERTC_DT1, BOOT_STATUS_MAGIC);
        ertc_bpr_data_write(ERTC_DT2, (uint32_t)Power_On);
    }
}

uint8_t Firmware0_Check;
#define Firmware0		((Firmware_Data_t*)(Firmware0_Base))

void IAP()
{
	SerialBoot_Initialize();
	Firmware0_Check = !Firmware_Check(Firmware0, 0);
	const int _s = 1000000;
	uint64_t until = Micros() + 1 * _s;
	
	while (((Micros() < until) && (serial_check() == 0) && serial_available() == 0)){}
	
	if (serial_check() == 0)
	{
		if(Firmware0_Check)
		{
			SetBootStatus(Shifting0);
			ResetMCU();
		}	
	}
	
	int Count_SerialBoot_Connect_Flag = 0;
	
	until = Micros() + 10 * _s;
	
	while (Micros() < until)
	{
		if (serial_available())
		{
			until = Micros() + 5000000;
			int value = serial_read();
			if (value >= 0)
			{
					if(value == SerialBoot_Connect_Flag)
					{
						Count_SerialBoot_Connect_Flag++;
						if(Count_SerialBoot_Connect_Flag >= 100)
						{
							ResetMCU();
						}
					}
					else
					{
						Count_SerialBoot_Connect_Flag = 0;
					}
					SerialBoot_Receive(value);
			}
		}
	}
	ResetMCU();
	while(1);
}
