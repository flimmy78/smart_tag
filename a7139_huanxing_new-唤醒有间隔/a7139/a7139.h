
#ifndef _A7139_H
#define _A7139_H
#include "stm32f0xx.h"
#include "main.h"
#define A7139_RFEN_PIN 	 GPIO_Pin_7
#define A7139_RFEN_PORT  GPIOA
#define A7139_RFEN_CLK	 RCC_AHBPeriph_GPIOA

#define A7139_SCS_PIN 	 GPIO_Pin_4
#define A7139_SCS_PORT 	 GPIOA
#define A7139_SCS_CLK		 RCC_AHBPeriph_GPIOA

#define A7139_SCK_PIN    GPIO_Pin_5
#define A7139_SCK_PORT   GPIOA
#define A7139_SCK_CLK    RCC_AHBPeriph_GPIOA

#define A7139_SDIO_PIN   GPIO_Pin_6
#define A7139_SDIO_PORT  GPIOA
#define A7139_SDIO_CLK   RCC_AHBPeriph_GPIOA

#define A7139_GDO1_PIN 	 GPIO_Pin_0
#define A7139_GDO1_PORT  GPIOA
#define A7139_GDO1_CLK   RCC_AHBPeriph_GPIOA

#define A7139_GDO2_PIN 	 GPIO_Pin_10
#define A7139_GDO2_PORT  GPIOB
#define A7139_GDO2_CLK   RCC_AHBPeriph_GPIOB

#define A7139_GDO1_EXTI_SOURCE    EXTI_PortSourceGPIOA
#define A7139_GDO1_EXTI_PIN   		EXTI_PinSource0
#define A7139_GDO1_EXTI_LINE      EXTI_Line0

#define A7139_GDO2_EXTI_SOURCE    EXTI_PortSourceGPIOB
#define A7139_GDO2_EXTI_PIN   		EXTI_PinSource10
#define A7139_GDO2_EXTI_LINE      EXTI_Line10
#define A7139_IRQn                EXTI4_15_IRQn

typedef enum 
{
  SCS = 0,
  SCK = 1,
  SDIO = 2,
	GIO1 = 3,
	GIO2 = 4,
	RFEN = 5
} A7139_IO_TypeDef;

extern  uint16  A7139Config[];
extern  uint16  A7139Config_PageA[];
extern  uint16  A7139Config_PageB[];


void A7139_IO_SetBit(A7139_IO_TypeDef io);

void A7139_IO_ReSetBit(A7139_IO_TypeDef io);

uint8 A7139_IO_Read_Bit(A7139_IO_TypeDef io);

void ByteSend(uint8 src);
uint8 ByteRead(void);
void StrobeCMD(uint8 cmd);


void A7139_WriteReg(uint8 address, uint16 dataWord);
uint16 A7139_ReadReg(uint8 address);
void A7139_WritePageA(uint8 address, uint16 dataWord);
uint16 A7139_ReadPageA(uint8 address);

void a7139_GPIO_Init(void);
void A7139_Config(void);
void A7139_WriteID(void);
void A7139_Cal(void);
void A7139_Exti_Init(void);
void A7139_RX_Exti(FunctionalState state);

#define ERR_GET_RSSI		0x00

















#endif
