//=============================================================================

//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "bsp.h"

#include "a7139.h"
#include "a7139_api.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void SysTime_handle(void);
void SysTick_Handler(void)
{
   SysTime_handle();
}

void EXTI0_1_IRQHandler(void)
{
	if(system_flag.sys_wor_sleep) WakeUp_Init();
	if(EXTI_GetITStatus(A7139_GDO1_EXTI_LINE) != RESET)
  {
		a7139_recv_handle();
		EXTI_ClearITPendingBit(A7139_GDO1_EXTI_LINE);
  }
	
	if(EXTI_GetITStatus(KEY3_EXTI_LINE) != RESET)
  {
		if(!system_flag.key_protect ) system_flag.key_press |= (0x01<<3);
		EXTI_ClearITPendingBit(KEY3_EXTI_LINE);
  }
}

void EXTI2_3_IRQHandler(void)
{
	if(system_flag.sys_wor_sleep) WakeUp_Init();
	if(EXTI_GetITStatus(KEY3_EXTI_LINE) != RESET)
  {
		if(!system_flag.key_protect ) system_flag.key_press |= (0x01<<3);
		EXTI_ClearITPendingBit(KEY3_EXTI_LINE);
  }
	system_flag.wakeup_except_rtc = 1;
}



void EXTI4_15_IRQHandler(void)
{
	if(system_flag.sys_wor_sleep) WakeUp_Init();
	
	if(EXTI_GetITStatus(KEY_PUT_EXTI_LINE) != RESET)
  {
		if(!system_flag.key_protect ) system_flag.key_press |= (0x01<<0);
		EXTI_ClearITPendingBit(KEY_PUT_EXTI_LINE);
  }
	if(EXTI_GetITStatus(KEY1_EXTI_LINE) != RESET)
  {
	if(!system_flag.key_protect ) system_flag.key_press |= (0x01<<1);
		EXTI_ClearITPendingBit(KEY1_EXTI_LINE);
  }
	if(EXTI_GetITStatus(KEY2_EXTI_LINE) != RESET)
  {
		if(!system_flag.key_protect ) system_flag.key_press |= (0x01<<2);
		EXTI_ClearITPendingBit(KEY2_EXTI_LINE);
  }
	
	system_flag.wakeup_except_rtc = 1;

}


void RTC_IRQHandler(void)
{ 
 if(system_flag.sys_wor_sleep) WakeUp_Init();
 if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
	 
		system_flag.wakeup_except_rtc = 0;
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
  } 
}


/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0x8.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
