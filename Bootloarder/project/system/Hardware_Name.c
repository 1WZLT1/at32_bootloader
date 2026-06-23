#include "stdint.h"
#include "Hardware_Name.h"

static const uint8_t Hardware_Name[] = "DETA-1";

int Get_Hardware_Name(uint8_t *buffer)
{
	int str_len=sizeof(Hardware_Name)-1;//exclude '\0'
	for (int i = 0; i < str_len; i++)
	{
		buffer[i] = Hardware_Name[i];
	}
	return str_len;
}

#define AT32_UID_BASE_ADDR       0x1FFFF7E8UL
#define AT32_FLASH_SIZE_ADDR     0x1FFFF7E0UL

#define FDI_CHIP_AT32F435        0x0435U

uint32_t FDI_Get_ChipID_128(uint16_t product, uint32_t *buf)
{
    const volatile uint32_t *chip_id =
        (const volatile uint32_t *)AT32_UID_BASE_ADDR;

    uint32_t flash_size_kb;
    uint16_t code;

    /*
     * F_SIZE低16位表示Flash容量，单位KB。
     */
    flash_size_kb =
        (*(const volatile uint32_t *)AT32_FLASH_SIZE_ADDR) &
        0xFFFFU;
    code = FDI_CHIP_AT32F435;


    /* AT32提供96位UID */
    buf[0] = chip_id[0];
    buf[1] = chip_id[1];
    buf[2] = chip_id[2];

    /*
     * 高16位：产品编号
     * 低16位：Bootloader自定义芯片编号
     */
    buf[3] = ((uint32_t)product << 16) |
             (uint32_t)code;

    return flash_size_kb * 1024U;
}
