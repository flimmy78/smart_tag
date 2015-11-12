/*
 * app_typedef.h
 *
 *  Created on: 2014-3-28
 *      Author: Jacques
 */

#ifndef APP_TYPEDEF_H_
#define APP_TYPEDEF_H_

#include "app_config.h"


#define TRUE 1
#define FALSE 0

/*Defines of sys paras*/
#define RFID_GATEWAY	        ((uint8)1)

#define RFID_NODE     ((uint8)0xB2) 


/*Defines for Node connecting status*/
#define TAG_STAT_NORMAL	        ((uint8)0)
#define TAG_EVENING_MODE_RUN    ((uint8)1)
#define TAG_HOLD_FOR_EVE_MODE   ((uint8)2)

#define TAIL										((uint8)0xAA)

/*Defines for Netwoking of Default values.*/
#define BROADCAST								((uint16)0xFFFF)
#define DEFAULT_PANID	     		  BROADCAST
#define DEFAULT_SHORTADDR				BROADCAST

#define DEFAULT_BUSS_CHN	     		  1
#define DEFAULT_BUGL_CHN			      2

/*Defines for block id for flash storing blocks*/
#define NUM_BLOCK_BASE_NO      ((uint8)20) //数字字模
#define DEC_BLOCK_NO           ((uint8)21)//小数点
#define UNIT_BLOCK_NO          ((uint8)22)//单位块

#define MAX_CELL_DATA           ((uint8)35)
#define MAX_BLOCK_LENGTH        ((uint16)0x200)

#define MAX_USR_GUI_NO          ((uint8)95)

/*Defines of external flash cells.*/
#define FLASH_WORD_SIZE         ((uint8)0x04)
#define FLASH_PAGE_SIZE         ((uint16)0x0400)
#define FLASH_WRITE_ADDR_OFFSET ((uint16)0x0200)

#define SYS_PARA_MAGIC_NUM                      ((uint16)0xA1A4)

typedef enum{NO=0,YES=!NO}STATUES_T;

typedef enum
{
    MATRIX_CMD = 0x00,
    GUI_DATA,
}REFRESH_CMD_TYPE; //网关刷屏数据类型

/*Defines for last rest <boot> source*/
typedef enum
{ 
    SOURCE_WATCHDOG_TIMEOUT = 1,
    SOURCE_FIEMWARE_UPDATE,
    SOURCE_HARD_REBOOT,
}REBOOR_SOURCE_TYPE;

typedef enum
{
    SYS_INIT           = 0xA0,
    
    NWK_FIND_INIT      = 0xA1,
    NWK_FIND_RUN       = 0xB1,
    NWK_JOIN_INIT      = 0xA2,
    NWK_JOIN_RUN       = 0xB2,
    
		LISTEN_INIT        = 0xAA,
    LISTEN_RUN         = 0xBB,
	
    STANDBY_INIT       = 0xA3,
    STANDBY_RUN	       = 0xB3,
    
    WRITE_INIT	       = 0xA5,
    WRITE_RUN	       = 0xB5,
    
    AKWAKE_LISTEN      = 0xA7,
    
    IDLE_RUN           = 0xA8,
    HOLD_NEXT_FIND     = 0xA9,
    
    NIGHT_MODE      = 0xAB,
    
}TAG_STATE_MACHINE; //状态机

typedef enum
{
   CMD_TURN_ON_LED = 0x00,
   CMD_TURN_OFF_LED= 0x01,
   CMD_LOAD_GUI    = 0x02,
   CMD_BLOCK_DISP  = 0x03,
   CMD_LED_SETUP   = 0x04,
   CMD_SLEEPTIME   = 0x05,
   CMD_BUTT_REPORT = 0x06,
   CMD_NIGHT_MODE  = 0x07,
   CMD_REBIND_NET  = 0x08,
   CMD_OFFLINE_NET = 0x09,
}ENUM_EVENT_CODE;

typedef enum
{
	 EVENT_INVENTORYMODE_CONFIRM = 0x04,//盘点确认
	
	 EVENT_KEY_PUT_PRESS 			= 0x10,
	 EVENT_KEY1_PRESS    			= 0x11, //按键1动作
   EVENT_KEY2_PRESS    			= 0x12, //按键2动作
   EVENT_KEY3_PRESS    			= 0x13, //按键3动作
	
	 EVENT_KEY_PUT_1_PRESS    = 0x14, //按键3动作
	 EVENT_KEY_PUT_2_PRESS    = 0x15, //按键3动作
	 EVENT_KEY_PUT_3_PRESS    = 0x16, //按键3动作
	 EVENT_KEY1_2_PRESS       = 0x17, //按键3动作
   EVENT_KEY1_3_PRESS       = 0x18, //按键3动作
 	 EVENT_KEY2_3_PRESS       = 0x19, //按键3动作

   MAX_REPORT_USR_EVENT_NUM = 0x20, //动作总数
}ENUM_USER_EVENT_CODE;

/*Defines for basic event code.*/
typedef enum
{
    CMD_LONG_BUSSI_DATA      = 0xa0,
    CMD_SHORT_BUSSI_DATA     = 0xa1, 
    CMD_UPDATE_FIRMWARE      = 0xc4,
    CMD_REPORT_TAG_DELIVER_MSG   = 0xbd,
}EVENT_CMD_TYPE;//数据事件命令码

typedef enum
{
    CMD_REQUEST_HEAD   = 0xc5c4,
    CMD_RESPONSE_HEAD  = 0xb5b4,
    NWK_REQUEST_HEAD   = 0xe5e4,
    NWK_RESPONSE_HEAD  = 0xd5d4,
}FRAME_HEAD_CODE;

/*Defines for frame types in APP lever.*/
typedef enum
{
    NWK_SEARCH  = 0xf0,
    NWK_JOIN    = 0xf1,
    NWK_QUERY   = 0xf2,
    NWK_RETRANS = 0xf3,
    NWK_RESP    = 0xf4,
}SEND_PACKET_TYPE;//协议命令码

/*Defines for FCF for Zigbee of Link layer */
typedef enum
{
    FCF_ACK_SRC_LONG_DATA 	= 0xc861,
    FCF_NOACK_SRC_LONG_DATA	= 0xc841,
    FCF_NOACK_SRC_SHORT_DATA 	= 0x8841,
    FCF_ACK_SRC_SHORT_DATA	= 0x8861,
	  FCF_WAKE_UP_DATA = 			0xedf4,
#ifdef NO_QUERY_REQ                       //只做心跳包，不作为查询包
    FCK_QUERY_MSG_ALONG         = 0x8b41,
#else
    FCK_QUERY_MSG_ALONG         = FCF_NOACK_SRC_SHORT_DATA,
#endif
    
}FCF_FRAME_FILL_CMD;

/*Defines of codes for Event exec. report to server.*/
typedef enum
{
   FLAG_LONG_DATA   = 0, //0. 长数据类型标志位
   FLAG_SHORT_DATA  = 1, //1：捎带数据标志位
   FLAG_FMWARE_OK   = 2, //8. 更新接收成功标志位
   FLAG_FMWARE_FAIL = 3, //9. 更新接收失败标志位
   FLAG_TRANS_REPORT= 4, //上报用户事件标志位    

//注意：最高位（即15位为拍灯标志位，应当保留）
}ACK_FLAG_T;

/*Defines of server returns exception types*/
typedef enum
{
  BUZY_LATANCY= 0x01,
  BAD_REQUEST = 0x02,
}EXCEPTION_REASON_T;

/*Defines of code of Event cmd noticing from gateway.*/
typedef enum
{   
    DATATYPE_NULL_JOB      	= 0x00,
    DATATYPE_SCREEN_DATE   	= 0x01,
    DATATYPE_FIREWARE_DATA 	= 0x02,
    TAG_KICKE_OUT_FROM_NET 	= 0x03,
    EXCEPTION_NOTICE_FLAG	= 0x04,
}QUERY_DATA_RESPONSE_TYPE;  //return DataType of query

typedef enum
{
    TRANS_SUCCESS,
    TRANS_FAILED,
}TRANSIMIT_STATUS;

/*tag_state attributes in running procedure.*/

#pragma pack (1)


typedef struct {
    //receive addr
    uint16 srcAddr;
    //receive frame control field
    uint16 fcf;
    //receive seqnum
    uint8 seqNumber;
    //rssi
    int16 rssi;
    //others
    uint8 ApduLen;
    uint8 MpduPayload[MAX_MPDU_LEN];
    uint8 *pApduPayload;
} BasicRfRxInfo_t, *pBasicRfRxInfo_t;

typedef struct
{
	BasicRfRxInfo_t  MpduBuff[MAX_MPDU_RXBUFF_NUM];
	uint8        	 MpduNum;
	uint8        	 CurrentMpdu;
}Mpdu_RxBuff_t, *pMpdu_RxBuff_t;

typedef struct {
    //panid
    uint16 DestPanId;
    //addr
    uint16 DestshortAddr;
    //frame control field
    uint16 fcf;
    //seqnum
    uint8 txSeqNumber;
    //others
    uint8 ApduLen;
    uint8 ApduPayload[MAX_MPDU_LEN];
    uint8 MpduPayload[MAX_MPDU_LEN];
} Mpdu_TxBuff_t, *pMpdu_TxBuff_t;


typedef struct {
	uint32 frameCounter;
	uint8 sendresult;
	bool  receiveOn;
	bool  ackReceived;
} BasicRfTxState_t, *pBasicRfTxState_t;

typedef struct
{
  uint8  type;
  uint8  spark_flag;
  uint16 on_ms;
  uint16 off_ms;
  uint16 spark_t;
  uint16 spark_ct;
}led_desc;  //led descriptor



typedef struct
{ 
	uint8  state;         //current state
	uint8  status;        //keep touch status to Gateway,
	
	uint16 waketime;      //tag_state's wait time after sleep
	uint16 waittime;      //tag_state's wait time after turn current state.
	
	uint32 sleeptime_count;
	uint32 sleeptime;     //sleeping time(s).
	uint32 cfg_sleeptime; //sleeptime of up layer cfged.
	
	uint16 check_cycle_s; //
	
	uint8 buz_type;
	uint8 buz_cmd_type;
        
	uint16  token;         //pan token for communication, 
	uint8  energy;        //energy value.
        
	bool   sleepflag;     //for ref. of sleep action
	bool   sendflag;      //for ref. of send a frame in a certain machine state.
	bool   recvflag;
        
	uint16  ackflag;      //ack event to be reported to gateway <using bit offset> 
	int8   checkfactor;   //in state of JOIN_RUN is seen as join retry times.\
                              and in STANDBY_RUN state is seen as times of NOT received\
                              response from gateway.    
        
	led_desc  led;           //type of led.
 
	
	bool  erase_flag;  //display GUI switch flag in lost gateway
         
	uint8 reportflag;
	uint8 cur_code;   //当前上报的用户事件 【0-MAX_REPORT_USR_EVENT_NUM】
	uint8 eve_counter[MAX_REPORT_USR_EVENT_NUM];
        
	uint16 shared_area[3];//temp stored variable as comman store area.
	/*screen para*/
	uint8 current_screen;
	uint8 is_need_load_last_screen;
}app_tag_state_t, *papp_rfid_t;

/*Define for flash store format for sys pan running paras.*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint8 blockid;
	uint8 screen_id;
	uint8 flash_page;
	uint16 flash_offset;
	uint16 blocksize;
}idx_table_t,*pidx_table_t;//display block storing indexer.

typedef struct
{
	uint16 magic_num;
	uint8 block_num;
	idx_table_t idx_tb[MAX_CELL_DATA]; 
}block_idx_info_t,*pblock_idx_info_t; //table of display block storing indexer.

#define QRCODE
/////////////////////////////////////////
typedef struct
{
	uint16 net_flag_set;
	uint16 pan_id;
	uint16 short_addr;
	uint16 gateway_addr;
	uint16 listen_channel;
	uint16 work_channel;
	
	uint16 update_software_flag;
	uint16 software_crc;
	
	uint16  last_gui_flag_set;
	uint8   last_gui_screen_id;
	
	uint16 block_idx_flag_set;
	uint8  block_num;
	idx_table_t idx_tb[MAX_CELL_DATA]; 
#ifdef QRCODE
	uint16 qr_set_flag;
	uint16 qr_crc;
	uint8  qrcode[63];
#endif
	
}tag_flash_info_t, *ptag_flash_info_t; //table of display block storing indexer.


typedef struct
{    
	uint8 total_len;
	uint16 head;
	uint16 panid;
	uint8  tag_cnt;
}wake_header_t, *pwake_header_t; 


typedef struct
{
	uint16 ackflag_set;
	uint16 cp_of_ackflag;
      
	uint16 sleeptime_set;
	uint32 sleeptime;
      
	uint16 led_set;
	led_desc  led;
      
	uint16 reportflag_set;
	uint8 reportflag;
	
	//buss_fuction pandian
	uint8 pandian_num_block_id;
	uint32 pandian_num_block_num;
	
	uint16 gui_screen_set;
	uint8 gui_screen_num;
	uint8 current_gui_page;
}app_state_para_t,*papp_state_para_t;


typedef struct
{
	uint16 head;    //type of header
	uint8  cmd;     //command type
	uint8  length;  //length of body,not including header.
}rf_header_t, *prf_header_t;

typedef struct
{
	uint16 length;    //length of buffer commanded to write.
	uint16 writelen;  //current write length
	uint8  sliceseq;  //current receive slice
	uint8  buf[SCREEN_RECVBUF_SIZE]; 
}wr_screen_t, *pwr_screen_t;

typedef struct
{
	uint32  length;    //length of buffer commanded to write.
	uint32  writelen;  //current write length
	uint16  sliceseq;  //current receive slice
	uint32  recv_check_sum; /*check_sum for firmware with add-sum*/
	uint8   soft_version;
}wr_firmware_t, *pwr_firmware_t;

/*Defines for ack structure to gateway of exec. result.*/
typedef struct
{
        uint16 token;
        uint8 ackCmd;
	uint8 status;
	uint8 tail;
}rf_rsp_t, *prf_rsp_t;

typedef struct
{
        uint16 token;
        uint8 ackCmd;
        uint8 event_type;
        uint8 event_cmd_type;
	//uint8 led_status;
	uint8 status;
	uint16 cost_time;
	uint8 tail;
}rf_buz_rsp_t, *prf_buz_rsp_t;

typedef struct
{
        uint8 seq_no;
        uint8 wait_flag;
}bz_resp_t, *pbz_resp_t;//业务层的重传信息

typedef struct
{
        uint16 token;
        uint8 ackCmd;
        uint8 bz_seqno;
        uint8 sendcode;
        uint8 len;
	uint8 common_data[MAX_TRANS_PICKING_DATA_LEN];
}rf_pr_rsp_t, *prf_pr_rsp_t;


typedef struct
{
	uint16 miss_rate;
	uint16  dbm;
	
	uint8 test_mode_key;
	uint16 test_send_n;
	uint16 test_recv_n;
}sys_test_t;

#pragma pack ()
extern app_tag_state_t tag_state;

#endif /* APP_TYPEDEF_H_ */
