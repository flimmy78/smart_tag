
#ifndef _BSP_H
#define _BSP_H

#include "stm32f0xx.h"
#include "stm32f0xx_flash.h"

#include "main.h"
#include "ep12864.h"

#include "app_typedef.h"
#include "app_config.h"

#include "app_state.h"
#include "app_flash.h"
#include "app_algo.h"
#include "a7139_api.h"
#include "app_msg.h"
#include "app_gui.h"
#include "app_wrscreen.h"

#include "a7139_api.h"

#include "pkt_build.h"

#include "app_key.h"


/////
#define  KEY_PUT_PIN 			 	   GPIO_Pin_15
#define  KEY_PUT_PORT				 	 GPIOA
#define  KEY_PUT_EXTI_SOURCE   EXTI_PortSourceGPIOA
#define  KEY_PUT_EXTI_PIN   	 EXTI_PinSource15
#define  KEY_PUT_EXTI_LINE     EXTI_Line15
#define  KEY_PUT_IRQn          EXTI4_15_IRQn

#define  KEY1_PIN 						 GPIO_Pin_5
#define  KEY1_PORT			 			 GPIOB
#define  KEY1_EXTI_SOURCE  		 EXTI_PortSourceGPIOB
#define  KEY1_EXTI_PIN   	     EXTI_PinSource5
#define  KEY1_EXTI_LINE  			 EXTI_Line5
#define  KEY1_IRQn       			 EXTI4_15_IRQn

#define  KEY2_PIN 						 GPIO_Pin_4
#define  KEY2_PORT						 GPIOB
#define  KEY2_EXTI_SOURCE  		 EXTI_PortSourceGPIOB
#define  KEY2_EXTI_PIN   	     EXTI_PinSource4
#define  KEY2_EXTI_LINE  			 EXTI_Line4
#define  KEY2_IRQn       			 EXTI4_15_IRQn

#define  KEY3_PIN 			 			 GPIO_Pin_3
#define  KEY3_PORT			 			 GPIOB
#define  KEY3_EXTI_SOURCE  		 EXTI_PortSourceGPIOB
#define  KEY3_EXTI_PIN   	     EXTI_PinSource3
#define  KEY3_EXTI_LINE  			 EXTI_Line3
#define  KEY3_IRQn       			 EXTI4_15_IRQn


typedef enum 
{
  KEY_PUT = 0,
  KEY1 = 1,
  KEY2 = 2,
  KEY3 = 3
} KEY_TypeDef;

typedef enum 
{
	KEY_PUT_1 = 4,
	KEY_PUT_2 = 5,
	KEY_PUT_3 = 6,
  KEY1_2 		= 7,
  KEY1_3 		= 8,
  KEY2_3		= 9
} KEY_MULT_TypeDef;


////


#define  LED_BLUE_PIN 			 GPIO_Pin_9
#define  LED_BLUE_PORT			 GPIOB
#define  LED_BLUE_CLK 			 RCC_AHBPeriph_GPIOB

#define  LED_YELLOW_PIN 			 GPIO_Pin_8
#define  LED_YELLOW_PORT			 GPIOB
#define  LED_YELLOW_CLK 			 RCC_AHBPeriph_GPIOB

#define  LED_RED_PIN 			 GPIO_Pin_7
#define  LED_RED_PORT			 GPIOB
#define  LED_RED_CLK 			 RCC_AHBPeriph_GPIOB

#define  LED_GREEN_PIN 			 GPIO_Pin_6
#define  LED_GREEN_PORT			 GPIOB
#define  LED_GREEN_CLK 			 RCC_AHBPeriph_GPIOB


typedef enum 
{
  LED_RED = 0,
  LED_GREEN  = 1,
  LED_YELLOW  = 2,
  LED_WHITE  = 3
} LED_TypeDef;

//////////



#define  EP12864_SID_PIN 			 GPIO_Pin_15
#define  EP12864_SID_PORT			 GPIOB
#define  EP12864_SID_CLK 			 RCC_AHBPeriph_GPIOB

#define  EP12864_SCLK_PIN 			 GPIO_Pin_13
#define  EP12864_SCLK_PORT 			 GPIOB
#define  EP12864_SCLK_CLK			 RCC_AHBPeriph_GPIOB

#define  EP12864_RES_PIN 			 GPIO_Pin_11
#define  EP12864_RES_PORT			 GPIOA
#define  EP12864_RES_CLK			 RCC_AHBPeriph_GPIOA

#define  EP12864_CS_PIN				GPIO_Pin_12
#define  EP12864_CS_PORT 			GPIOB
#define  EP12864_CS_CLK 				RCC_AHBPeriph_GPIOB

#define  EP12864_RS_PIN   			 GPIO_Pin_10
#define  EP12864_RS_PORT 			 GPIOA
#define  EP12864_RS_CLK  			 RCC_AHBPeriph_GPIOA

#define  EP12864_BUSY_PIN   			 GPIO_Pin_9
#define  EP12864_BUSY_PORT 			 GPIOA
#define  EP12864_BUSY_CLK  			 RCC_AHBPeriph_GPIOA

#define EP12864_IS_BUSY() 			GPIO_ReadInputDataBit(EP12864_BUSY_PORT,EP12864_BUSY_PIN)

typedef enum 
{
  SID = 0,
  SCLK = 1,
  RES = 2,
  CS = 3,
  RS = 4
} EP12864_IO_TypeDef;


void EP12865_IO_Init(EP12864_IO_TypeDef io);
void EP12864_IO_SetBit(EP12864_IO_TypeDef io);

void EP12864_IO_RstBit(EP12864_IO_TypeDef io);
void EP12864_Init(void);

#define LED_OFF_SLEEP_TIME 1
#pragma pack (1)

typedef struct
{
	uint32 sysTimeCount;
	uint32 sysTimeDelay;
	
	 uint8 led_state;
	
	uint8 sys_wor_sleep;
	uint8 sys_wake_up;
	
	uint8 key_protect;
	uint8 key_press;
	
	uint32 TagTimeCount;
	uint32 TagTimeTimeout;
	
	uint32 sys_power;
	uint32 power_last_time;
	uint32 power_detect_time;
	
	uint32 led_on_time;
	uint32  rf_on_time;
	uint32 long_date_wait_time;
	
	uint8  wakeup_except_rtc;
	
	uint8	 sys_pwr_time_sec;
	uint8	 led_pwr_time_sec;
	uint8	  rf_pwr_time_sec;
	
	uint16 rssi_high_mask;//RSSI ·§Öµ
	uint16 rssi_low_mask;//RSSI ·§Öµ
}system_flag_t;

#pragma pack ()


extern system_flag_t system_flag;

void Board_Init(void);


uint8 Key_ReadValue(KEY_TypeDef key);


REBOOR_SOURCE_TYPE Get_RstType(void);
void SystemSleep_s(uint32 sec);
void GPIO_Exti_Init(void);
void Delay_ms(uint32 nMs);
void system_reset(void);


#endif
