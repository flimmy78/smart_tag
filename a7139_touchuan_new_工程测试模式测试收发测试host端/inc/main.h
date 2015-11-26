
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

#define DATA_FROM_USART_BUFFSIZE  30
#define PKT_MAX_LEN               64


/*****Define channel ******/
#define MAX_CHANNEL 60
#define FREQ_433  0x0021D405
#define CHANLE_FP_DIFF 0x1400// 0x500

#define FRAME_PKT_LEN    64


#define PKT_CONFIG_MAX_LEN               8

#define TAG_REPORT_LIVING_TIME 120000//120s

#define DEFAULT_JOIN_CHN  0
#define DEFAULT_BUSS_CHN  1

#define DEFAULT_PANID     0xffff
#define DEFAULT_GW        0xffff
typedef enum{
	C_SYNC_HEAD  = 0x5A,
	C_SYNC_TAIL  = 0x5B,
	
	C_INIT_CMD   = 0x00,
	C_SEND_CMD   = 0x11,
	C_REMOVE_CMD = 0x22,
	
	C_RECV_REPLY = 0x66,
	C_CFG_REPLY  = 0x77,
	C_NOTE_REPLY = 0x88,
	C_TMOUT_REPLY= 0x99,
}FRAME_CODE_T;

#pragma pack (1)
typedef struct {
    uint16 shortAddr;
    uint16 panId;
    uint8 channel;
} BasicRfCfg_t;

typedef struct
{
	uint8  recvdata[DATA_FROM_USART_BUFFSIZE][PKT_MAX_LEN + 2];
	uint8  pktcnt;
	uint8  currentData;
}Data_from_Uart_to_Rf_t;

typedef struct{
  uint8 head;
  uint8 cmd;
  uint16 addr;
  uint16 U16_info;
  uint8 U8_info;
  uint8 tail;
}pkt_master_t;

#pragma pack ()
extern BasicRfCfg_t RfConfig;
extern Data_from_Uart_to_Rf_t Data_from_Uart_buffer;
extern Data_from_Uart_to_Rf_t Data_from_Rf_buffer;

#endif
