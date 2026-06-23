/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f435_437_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f435_437_int.h"
/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "ring_buffer.h"
#include "uart_serve.h"
#include "wk_dma.h"
#include "SerialBoot.h"
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

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */
uint32_t Tick_Us_High = 0;
uint8_t  tmr5_first_bk = 0;
int64_t CortexM_Get_Us(void)
{
	while(1)
	{
		volatile uint32_t tick_0  = TMR5->cval;
		volatile uint32_t tick_32 = Tick_Us_High;
		if(tick_0 > TMR5->cval)
			continue;
		return ((int64_t) tick_32 << 32) | tick_0;
	}
}
/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

  /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */

    /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
  /* add user code begin PendSV_IRQ 0 */

  /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */

  /* add user code end PendSV_IRQ 1 */
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  /* add user code begin SysTick_IRQ 0 */

  /* add user code end SysTick_IRQ 0 */


  /* add user code begin SysTick_IRQ 1 */

  /* add user code end SysTick_IRQ 1 */
}

/**
  * @brief  this function handles USART1 handler.
  * @param  none
  * @retval none
  */
void USART1_IRQHandler(void)
{
  /* add user code begin USART1_IRQ 0 */
	if(USART1->sts_bit.idlef == 1)
	{
		usart_flag_clear(USART1,USART_IDLEF_FLAG);
		
		dma_channel_enable(DMA1_CHANNEL1, FALSE);
		uint32_t recv_len = USART2_RX_BUF_SIZE - DMA1_CHANNEL1->dtcnt;
	
		serial_receive(recv_len);
		
		wk_dma_channel_config(DMA1_CHANNEL1, 
                       (uint32_t)&USART1->dt, 
                       (uint32_t)&USART2_RX_BUF1, 
                       USART2_RX_BUF_SIZE);
	
		dma_channel_enable(DMA1_CHANNEL1, TRUE);	
	}
  /* add user code end USART1_IRQ 0 */
  /* add user code begin USART1_IRQ 1 */

  /* add user code end USART1_IRQ 1 */
}

/**
  * @brief  this function handles TMR5 handler.
  * @param  none
  * @retval none
  */
void TMR5_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR5_GLOBAL_IRQ 0 */
	if(TMR5->ists & 0x01)
	{
		Tick_Us_High++;
		if(!tmr5_first_bk)tmr5_first_bk = 1,Tick_Us_High -= 1;
	}
  /* add user code end TMR5_GLOBAL_IRQ 0 */


  /* add user code begin TMR5_GLOBAL_IRQ 1 */

  /* add user code end TMR5_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 1 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* add user code begin DMA1_Channel1_IRQ 0 */
	if (dma_flag_get(DMA1_FDT1_FLAG) != RESET)
  {
     dma_flag_clear(DMA1_FDT1_FLAG);
		 if (dma_flag_get(DMA1_FDT1_FLAG) != RESET)
			{
					dma_channel_enable(DMA1_CHANNEL1, FALSE);
					dma_flag_clear(DMA1_FDT1_FLAG);

					serial_receive(USART2_RX_BUF_SIZE);

					wk_dma_channel_config(
							DMA1_CHANNEL1,
							(uint32_t)&USART1->dt,
							(uint32_t)USART2_RX_BUF1,
							USART2_RX_BUF_SIZE);

					dma_channel_enable(DMA1_CHANNEL1, TRUE);
			}
	}
  /* add user code end DMA1_Channel1_IRQ 0 */
  /* add user code begin DMA1_Channel1_IRQ 1 */

  /* add user code end DMA1_Channel1_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 2 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* add user code begin DMA1_Channel2_IRQ 0 */
  if (dma_flag_get(DMA1_FDT2_FLAG) != RESET)
  {
      dma_flag_clear(DMA1_FDT2_FLAG);

      dma_channel_enable(DMA1_CHANNEL2, FALSE);

      /* 必须在这里清零 */
      usart1_tx_dma_busy = 0U;
  } 
  /* add user code end DMA1_Channel2_IRQ 0 */
  /* add user code begin DMA1_Channel2_IRQ 1 */

  /* add user code end DMA1_Channel2_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
