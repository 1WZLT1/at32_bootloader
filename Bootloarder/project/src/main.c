/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f435_437_wk_config.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_dma.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "uart_serve.h"
#include "IAP.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
typedef void (*AppEntry_t)(void);

static inline void BootStatus_Init(void)
{
    /* 开启电源控制 PWC 外设时钟 */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* 允许访问、写入后备域 */
    pwc_battery_powered_domain_access(TRUE);
}

#define BOOT_STATUS_MAGIC  0x12345678UL

static inline BootStatus_t GetBootStatus(void)
{
    uint32_t raw_status;

    /* 判断后备寄存器是否已经初始化 */
    if (ertc_bpr_data_read(ERTC_DT1) == BOOT_STATUS_MAGIC)
    {
        /* 读取保存的 Boot 状态 */
        raw_status = ertc_bpr_data_read(ERTC_DT2);

        /* 状态值超出枚举范围，说明数据无效 */
        if (raw_status >= (uint32_t)Boot_StatusMax)
        {
            ertc_bpr_data_write(ERTC_DT2, (uint32_t)Power_On);
            return Power_On;
        }

        return (BootStatus_t)raw_status;
    }
    else
    {
        /* 第一次使用或后备域数据已经丢失 */
        ertc_bpr_data_write(ERTC_DT1, BOOT_STATUS_MAGIC);
        ertc_bpr_data_write(ERTC_DT2, (uint32_t)Power_On);

        return Power_On;
    }
}
/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */
void Shift_Firmware(Firmware_Data_t *Firmware)
{
    uint32_t app_addr;
    uint32_t app_sp;
    uint32_t app_reset;

    app_addr  = 0x08010800;             // 0x08010800
    app_sp    = *(volatile uint32_t *)(app_addr + 0U);
    app_reset = *(volatile uint32_t *)(app_addr + 4U);
	
		__disable_irq();
	
		SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;
    
		for (uint32_t i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
		
		SetBootStatus(Power_On);
		
		SCB->VTOR = app_addr;
		__set_MSP(app_sp);
		
		__enable_irq();
		
		((AppEntry_t)app_reset)();
		while (1);
}

#define Firmware0		((Firmware_Data_t*)(Firmware0_Base))
/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
	BootStatus_Init();
	BootStatus_t status = GetBootStatus();
	int Firmware0_Check = 0;
	
	switch(status)
	{
		case Shifting0:	
			Firmware0_Check = !Firmware_Check(Firmware0, 0);
			if(Firmware0_Check)
				Shift_Firmware(Firmware0);
			break;
	}
  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        (uint32_t)&USART1->dt, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init dma1 channel2 */
  wk_dma1_channel2_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL2, 
                        (uint32_t)&USART1->dt, 
                        DMA1_CHANNEL2_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL2_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL2, TRUE);

  /* init usart1 function. */
  wk_usart1_init();

  /* init tmr5 function. */
  wk_tmr5_init();

  /* add user code begin 2 */
	dma_channel_enable(DMA1_CHANNEL1, FALSE);
	dma_channel_enable(DMA1_CHANNEL2, FALSE);
	UART_Serve_Init();
	IAP();
  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */

    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

  /* add user code end 4 */
