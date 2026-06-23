/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_tmr.c
  * @brief    work bench config program
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
#include "wk_tmr.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init tmr5 function.
  * @param  none
  * @retval none
  */
void wk_tmr5_init(void)
{
  /* add user code begin tmr5_init 0 */

  /* add user code end tmr5_init 0 */


  /* add user code begin tmr5_init 1 */

  /* add user code end tmr5_init 1 */

  /* configure plus mode */
  tmr_32_bit_function_enable(TMR5, TRUE);

  /* configure counter settings */
  tmr_cnt_dir_set(TMR5, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR5, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR5, FALSE);
  tmr_base_init(TMR5, 4294967295, 287);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR5, FALSE);
  tmr_primary_mode_select(TMR5, TMR_PRIMARY_SEL_RESET);

  tmr_counter_enable(TMR5, TRUE);

  /**
   * Users need to configure TMR5 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR5 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f435_437_int.c file.
   *     --void TMR5_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr5_init 2 */

  /* add user code end tmr5_init 2 */
}

/* add user code begin 1 */

/* add user code end 1 */
