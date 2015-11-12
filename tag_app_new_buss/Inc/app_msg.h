/*
 * app_msg.h
 *
 *  Created on: 2014-3-6
 *      Author: Jacques

 ***@breif:Common Define of data struct of msg sending for and from with gateway.*
 */

#ifndef APP_MSG_H_
#define APP_MSG_H_

#include "app_typedef.h"

typedef enum {MSG_SUCCESS = 0, MSG_FAILED = !MSG_SUCCESS} MSG_Status;
#pragma pack (1)
//prototype of tag network finding request Frame payload.
typedef struct
{
        uint8   devType;
	uint8   status;  //±êÇ©×ÔÉí×´Ì¬
	uint8   tail;
}app_search_req_t, *papp_search_req_t;

//prototype network finding response Frame payload from gateway.

typedef struct
{
	uint8   devType;
	uint16  nwkPanid;
        uint16  nwkAddr;
	uint8   tail;
}app_search_rsp_t, *papp_search_rsp_t;

//prototype of tag join request Frame payload.
typedef struct
{
	uint8   devType;
        uint8   firmware_version;
        uint32  sleeptime;
	uint8   tail;
}app_join_req_t,*papp_join_req_t;

//prototype of network join response Frame payload from gateway.
typedef struct
{
	uint8   devType;
	uint16  tagAddr;
	uint8   listen_channel;
  uint8   work_channel;
	uint8   tail;
}app_join_rsp_t,*papp_join_rsp_t;

//prototype of tag query request Frame payload.
typedef struct
{
        uint16  token;
	uint8   power;
	uint8   tail;
}app_standby_query_t,*papp_standby_query_t;

typedef struct
{
        uint16  token;
	uint16  store_slice;
	uint8   tail;
}app_retans_req_t,*papp_retans_req_t;

//prototype of network exec-report ack response Frame payload from gateway.
typedef struct
{
	uint8   ackCmd;
	uint8   more_buss_data;
	uint8   tail;
}app_rsp_ack_t,*papp_rsp_ack_t;

//prototype of network exception response Frame payload from gateway.
typedef struct
{
	uint8   exp_code;
	uint8   tail;
}app_rsp_excp_t,*papp_rsp_excp_t;

typedef struct
{
	uint8   dataType;
	uint8   tail;
}app_rsp_query_t,*papp_rsp_query_t;

#define MAX_CMD_DATA_LENGTH			15

//prototype of event type response Frame payload from gateway.
typedef struct
{
	uint8   eventCmd;
	uint8   eventMsg[MAX_CMD_DATA_LENGTH];
	uint8   tail;
}app_rsp_queryEvent_t,*papp_rsp_queryEvent_t;

//prototype of screen data info response Frame payload from gateway.
typedef struct
{
	uint16 length;
	uint8 tail;
}rf_wrscreen_info_t, *prf_wrscreen_info_t;

//prototype of firmware data info response Frame payload from gateway.
typedef struct
{
	uint32 length;
        uint32 check_sum;
        uint8  soft_version;
	uint8 tail;
}rf_firmware_info_t, *prf_firmware_info_t;
#pragma pack ()


void App_SendNetworkSearchMsg(void);
void App_Find_RecvProcess(void);
void App_RecvNetworkSearchMsgRsp(void);

void App_SendJoinRequestMsg(void);
void App_Join_RecvProcess(void);
void App_RecvJoinRequestMsgRsp(void);

uint8 App_RecvListenMsgRsp(void);

void App_SendEventRspMsg(uint8 idx);
uint8 App_SendEventRsp(uint8 cmd,uint8 status);

void App_SendQueryMsg(void);

void App_SendRetransReqMsg(void);

int  App_RecvStandByMsgRsp(void);
void App_Standby_RecvProcess(void);

void App_Write_RecvProcess(void);

void App_RecvWriteScreenReq(uint8 *pPayload, uint8 length);
void App_RecvWriteFirmwareToflash(uint8 *pdata, uint8 length);

extern wr_firmware_t firmware;

#endif /* APP_MSG_H_ */
