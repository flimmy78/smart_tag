


#ifndef _SLEEP_WAKE_H
#define _SLEEP_WAKE_H

#include "main.h"

typedef enum {WA_SUCESS = 0, WA_FAILED = !WA_SUCESS} WA_Status;
#define LIST_OFFSET(m,n) (n%m)

#define WAKE_TAIL  0xaa

#define WAKE_HEAD 0xedf4

#define TAG_MAX_SEND_CNT 26
#define TAG_SEND_TIMEOUT 360//320    ///(50ms*360)= 18s
#define TAG_REPORT_SEND_TIME 50   //50ms
#define TAG_REPORT_LIVING_TIME 30000   //30s

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
	uint8 tag_cnt;
	uint8 tag_timeout_cnt;
	uint16 short_addr[TAG_MAX_SEND_CNT];
	uint16 tag_wake_timecount[TAG_MAX_SEND_CNT];
}wake_tag_send_t, *pwake_tag_send_t; 

typedef struct{
  uint8 head;
  uint8 cmd;
  uint16 addr;
  uint16 U16_info;
  uint8 U8_info;
  uint8 tail;
}pkt_master_t;


typedef struct
{    
	uint8 total_len;
	uint16 head;
	uint16 panid;
	uint8  tag_cnt;
}wake_header_t, *pwake_header_t; 

#pragma pack ()


void Tag_Init(void);
void send_alive_cmd_to_master(void);
void SendWakeMsg(void);
#endif

