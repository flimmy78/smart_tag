
#include "bsp.h"
#include "a7139.h"
#include "a7139_api.h"

static void Sleep_Init(void)
{
  GPIO_InitTypeDef    GPIO_InitStruct;
	
  // Enable GPIOs clocks
  RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN | RCC_AHBENR_GPIOFEN);
	
  /*GPIOA LowPower mode*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All & (~(GPIO_Pin_13 | GPIO_Pin_14 | A7139_SCS_PIN | A7139_SCK_PIN | A7139_SDIO_PIN | A7139_GDO1_PIN 
																						 | KEY_PUT_PIN 
																						 | EP12864_RES_PIN | EP12864_RS_PIN | EP12864_BUSY_PIN));
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin =  A7139_SCS_PIN | A7139_SCK_PIN | A7139_SDIO_PIN;
												
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin =  EP12864_RES_PIN | EP12864_RS_PIN | EP12864_BUSY_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		
	/*GPIOB LowPower mode*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All &(~(A7139_GDO2_PIN 
																						| KEY1_PIN | KEY2_PIN | KEY3_PIN 
																						| EP12864_SID_PIN | EP12864_SCLK_PIN | EP12864_CS_PIN
																						| LED_RED_PIN | LED_GREEN_PIN | LED_YELLOW_PIN | LED_BLUE_PIN));
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	 	
	
	GPIO_InitStruct.GPIO_Pin =  EP12864_SID_PIN | EP12864_SCLK_PIN |  EP12864_CS_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin =  LED_RED_PIN | LED_GREEN_PIN | LED_YELLOW_PIN | LED_BLUE_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN ;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*GPIOC LowPower mode*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*GPIOC LowPower mode*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOD, &GPIO_InitStruct);	
	
	
	/*GPIOF LowPower mode*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOF, &GPIO_InitStruct);	
	
	RCC->AHBENR &= (uint32_t)(~(RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN | RCC_AHBENR_GPIOFEN));
}


void WakeUp_InitTEST(void)
{
	SystemInit();
  SysTick_Init();	
  
	Rcc_Init();
}

void WakeUp_Init(void)
{
	SystemInit();
  SysTick_Init();	
  
	Rcc_Init();
	
  LED_Init();
  Button_Init();
	
	EP12864_Init();
	Init_EPaper(1);
  lcd_sleep_mode();
	
	a7139_GPIO_Init();

	//GPIO_Exti_Init();
	//
#ifdef PRINT_DEBUG
	USART2_Init();
#endif
	
}


void SystemSleep_s(uint32 sec)
{
	if(sec>0)  generate_alarm_Interrupt(sec);
	/* low power configration*/
	a7139_entry_sleep_mode();
	Sleep_Init();
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	WakeUp_Init();
	a7139_wake_up_from_sleep_mode();
	if(sec > 0) cancel_alarm_Interrupt();
}


void SystemSleep_wakeBy_WorAndKey(void)
{
	
	WOR_enable_by_sync();

	sys_info_print("[Enter WOR]", 11);
	Sleep_Init();
	system_flag.sys_wor_sleep = 1;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	WakeUp_Init();
	
	cancel_alarm_Interrupt();
	
	WOR_disable();
	
	system_flag.sys_wor_sleep = 0;
}

