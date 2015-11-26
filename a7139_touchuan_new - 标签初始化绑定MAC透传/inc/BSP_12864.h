
#ifndef _BSP_12864_H
#define _BSP_12864_H

#include "stm32f0xx_gpio.h"

#define  EP12864_SID_PIN 			 GPIO_Pin_11
#define  EP12864_SID_PORT			 GPIOB
#define  EP12864_SID_CLK 			 RCC_AHBPeriph_GPIOB

#define  EP12864_SCLK_PIN 			 GPIO_Pin_10
#define  EP12864_SCLK_PORT 			 GPIOB
#define  EP12864_SCLK_CLK			 RCC_AHBPeriph_GPIOB

#define  EP12864_RES_PIN 			 GPIO_Pin_1
#define  EP12864_RES_PORT			 GPIOB
#define  EP12864_RES_CLK			 RCC_AHBPeriph_GPIOB

#define  EP12864_CS_PIN				GPIO_Pin_0
#define  EP12864_CS_PORT 			GPIOB
#define  EP12864_CS_CLK 				RCC_AHBPeriph_GPIOB

#define  EP12864_RS_PIN   			 GPIO_Pin_7
#define  EP12864_RS_PORT 			 GPIOA
#define  EP12864_RS_CLK  			 RCC_AHBPeriph_GPIOA

#define  EP12864_BUSY_PIN   			 GPIO_Pin_2
#define  EP12864_BUSY_PORT 			 GPIOB
#define  EP12864_BUSY_CLK  			 RCC_AHBPeriph_GPIOB

#define EP12864_IS_BUSY() 			GPIO_ReadInputDataBit(EP12864_BUSY_PORT,EP12864_BUSY_PIN)

typedef enum 
{
  SID = 0,
  SCLK = 1,
  RES = 2,
  CS = 3,
  RS = 4
} EP12864_IO_TypeDef;

#endif
