
#ifndef _A7139_H
#define _A7139_H
#include "main.h"

#define A7139_SCS_PIN 	GPIO_Pin_7
#define A7139_SCS_PORT 	GPIOB
#define A7139_SCS_CLK		RCC_AHBPeriph_GPIOB

#define A7139_SCK_PIN   GPIO_Pin_8
#define A7139_SCK_PORT  GPIOB
#define A7139_SCK_CLK   RCC_AHBPeriph_GPIOB

#define A7139_SDIO_PIN     GPIO_Pin_9
#define A7139_SDIO_PORT    GPIOB
#define A7139_SDIO_CLK     RCC_AHBPeriph_GPIOB

#define A7139_GDO1_PIN 					GPIO_Pin_3
#define A7139_GDO1_PORT 				GPIOB
#define A7139_GDO1_CLK  					RCC_AHBPeriph_GPIOB

#define A7139_GDO1_EXTI_SOURCE   EXTI_PortSourceGPIOA
#define A7139_GDO1_EXTI_PIN   		EXTI_PinSource0
#define A7139_GDO1_EXTI_LINE     EXTI_Line0

#define A7139_GDO2_PIN 					GPIO_Pin_4
#define A7139_GDO2_PORT 				GPIOB
#define A7139_GDO2_CLK  					RCC_AHBPeriph_GPIOB
#define A7139_GDO2_EXTI_SOURCE   EXTI_PortSourceGPIOB
#define A7139_GDO2_EXTI_PIN   		EXTI_PinSource4
#define A7139_GDO2_EXTI_LINE     EXTI_Line4
#define A7139_IRQn               EXTI4_15_IRQn

typedef enum 
{
  SCS = 0,
  SCK = 1,
  SDIO = 2,
	GIO1 = 3,
	GIO2 = 4
} A7139_IO_TypeDef;


extern  uint16  A7139Config[];
extern  uint16  A7139Config_PageA[];
extern  uint16  A7139Config_PageB[];

















#endif
