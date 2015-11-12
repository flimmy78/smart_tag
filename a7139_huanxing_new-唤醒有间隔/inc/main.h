
#ifndef _MAIN_H
#define _MAIN_H
#include "stm32f0xx.h"


typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef unsigned char   bool;


typedef enum {TRUE = 0, FALSE = !TRUE} APP_FLAG;

#define DATA_FROM_USART_BUFFSIZE  10

#define PKT_MAX_LEN               8

#define BROADCAST		((uint16)0xFFFF)

#define RF_DEFAULT_CHANNEL              ((uint8)10)  

/*****Define channel ******/
/*RF CHAN definition*/
#define FREQ_433_BUGL  0x0021D405 // 0x0021D405
#define FREQ_433_JOIN  0x0021E66B
#define FREQ_433_BUSS  0x0021F205

#define FRAME_PKT_LEN    64



#define DEFAULT_BUGL_CHN  2
#define DEFAULT_PANID     0x5a2b
#define DEFAULT_GW        0x2a01



#pragma pack (1)
typedef struct
{
	uint8  recvdata[DATA_FROM_USART_BUFFSIZE][PKT_MAX_LEN];
	uint8  pktcnt;
	uint8  currentData;
}Data_from_Uart_to_Rf_t;


#pragma pack ()

extern Data_from_Uart_to_Rf_t Data_from_Uart_buffer;

#endif
