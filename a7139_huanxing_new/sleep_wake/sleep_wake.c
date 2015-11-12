

#include "sleep_wake.h"

BasicRfCfg_t RfConfig = {0};

wake_tag_send_t wake_tag_list = {0};

uint8 send_state = 0;
uint8 send_msg_buf[64] = {0};

static void send_rf_cmd_to_master(pkt_master_t* resp);

void build_wakeup_msg(void);
void report_timeout_to_master(uint16 tagaddr);
void send_alive_cmd_to_master(void);


extern uint32   sysTime1ms;
extern uint8 tag_check_flag;
void tag_timeout_handler(void) //called per 1ms
{
		/*send  msg per 6ms*/
	if(sysTime1ms%6 == 0)
	{
		send_state = 1;
	}
	
	/*send timeout msg per 50ms*/
	if(sysTime1ms%TAG_REPORT_SEND_TIME == 0)
	{
		tag_check_flag = 1;
	}
	
	if(sysTime1ms%TAG_REPORT_LIVING_TIME == 0)
	{
		//send_alive_cmd_to_master();
	}
}



void Tag_state_check(void)
{
	int t;
	for(t=0;t<TAG_MAX_SEND_CNT;t++)
	{
		if(wake_tag_list.short_addr[t] != 0 && wake_tag_list.tag_wake_timecount[t]>0)
		{
			wake_tag_list.tag_wake_timecount[t] --;
			if(wake_tag_list.tag_wake_timecount[t] == 0)
			{
				wake_tag_list.tag_timeout_cnt ++;
				build_wakeup_msg();
		  }
     }
	}
	
	if(wake_tag_list.tag_timeout_cnt >0)
	{
			for(t=0;t<TAG_MAX_SEND_CNT;t++)
			{
				if(wake_tag_list.short_addr[t] != 0 && wake_tag_list.tag_wake_timecount[0] ==0)
				{
					report_timeout_to_master(wake_tag_list.short_addr[t]);
				}
			}
			
	}


}
void Tag_Init(void)
{
	RfConfig.channel =DEFAULT_BUGL_CHN;
	RfConfig.panId = DEFAULT_PANID;
	RfConfig.shortAddr = DEFAULT_GW;
	
	A7139_SetChn(RfConfig.channel);

}
void data_clean(void)
{
   memset(&RfConfig,0,sizeof(BasicRfCfg_t));
   memset((uint8*)&wake_tag_list.tag_cnt,0,sizeof(wake_tag_list));
   memset((uint8*)&send_msg_buf,0,64);
	send_state = 0;

}

WA_Status tag_send_list_add(uint16 tag_addr)
{
	int t;
	
	if(wake_tag_list.tag_cnt >= TAG_MAX_SEND_CNT || tag_addr == 0) return WA_FAILED;
	
	
	for(t=0;t<TAG_MAX_SEND_CNT;t++)
	{
		if(wake_tag_list.short_addr[t] == tag_addr)
		{
			return WA_FAILED;
		}
  }
	
	for(t=0;t<TAG_MAX_SEND_CNT;t++)
	{
		if(wake_tag_list.short_addr[t] == 0)
		{
			wake_tag_list.short_addr[t] = tag_addr;
			wake_tag_list.tag_cnt ++;
			wake_tag_list.tag_wake_timecount[t] = TAG_SEND_TIMEOUT;
			break;
		}
  }
	build_wakeup_msg();
	
  return WA_SUCESS;	
}

WA_Status tag_send_list_remove(uint16 tag_addr)
{
	int t;
	for(t = 0; t< TAG_MAX_SEND_CNT; t++)
	{
		if(wake_tag_list.short_addr[t] == tag_addr) 
		{
			wake_tag_list.tag_cnt --;
			if(wake_tag_list.tag_wake_timecount[t] == 0)wake_tag_list.tag_timeout_cnt --;
			wake_tag_list.short_addr[t] = 0;
			wake_tag_list.tag_wake_timecount[t] = 0;
			build_wakeup_msg();
			return WA_SUCESS;
    }
  }		
	
  return WA_FAILED;	
}



void SendWakeMsg(void)
{
   if(send_state == 1 && send_msg_buf[0] > 6)
	 {
		 send_state = 0;
		 A7139_Send_Msg(&send_msg_buf[0], send_msg_buf[0]+1); 		
	 }
}

void build_wakeup_msg(void)
{
	int i = 0;
	uint16 crc;
	uint8 tag_cnt_to_send = wake_tag_list.tag_cnt - wake_tag_list.tag_timeout_cnt;
	pwake_header_t header = (pwake_header_t)send_msg_buf;
	uint16* pdata = (uint16*)(&send_msg_buf[0] + sizeof(wake_header_t));
	
  uint8* tail = &send_msg_buf[0] + sizeof(wake_header_t) + tag_cnt_to_send*2;
	
	header->tag_cnt = wake_tag_list.tag_cnt - wake_tag_list.tag_timeout_cnt;
	
	header->total_len = 6 + (header->tag_cnt)*2;
	header->head = WAKE_HEAD;
	header->panid = RfConfig.panId;
	
      
	for(i=0;i<TAG_MAX_SEND_CNT; i++ )
	{
		if(wake_tag_list.short_addr[i] !=0 && wake_tag_list.tag_wake_timecount[i] != 0)
		{
			*pdata = wake_tag_list.short_addr[i];
			pdata++;
		}
	}
	*tail = WAKE_TAIL;
	crc = cal_crc16(send_msg_buf, header->total_len + 1);
	*(tail+1) = (uint8)crc;
	*(tail+2) = (uint8)(crc>>8);
	
	A7139_Fill_FIFO(&send_msg_buf[0], header->total_len + 1 + 2);

}
////////////////////////////////////

void cmd_handle(pkt_master_t* req)
{
  uint8 reply_sendbuf[10] = {0};
  uint8 ret = 0;
  
  pkt_master_t *resp = (pkt_master_t*)reply_sendbuf;
  resp->head = C_SYNC_HEAD;
  resp->tail = C_SYNC_TAIL;
  
  switch(req->cmd){
    case C_INIT_CMD:{
      
      data_clean();
      RfConfig.panId = req->U16_info;
      RfConfig.shortAddr = req->addr;
      RfConfig.channel = req->U8_info;
      
			A7139_SetChn(RfConfig.channel);
          
      resp->cmd  = C_CFG_REPLY;
      resp->addr = RfConfig.shortAddr;
      resp ->U8_info =  RfConfig.channel;
      resp ->U16_info = RfConfig.panId;
    }
    break;
                         
    case C_SEND_CMD:{
      uint8 ret = 0;
      ret = tag_send_list_add(req->addr);
      resp->cmd  = C_RECV_REPLY;
      resp->addr = req->addr;
      resp ->U8_info =  ret;                 
    }
    break;
                         
    case C_REMOVE_CMD:{
      uint8 ret = 0;
      ret = tag_send_list_remove(req->addr);	
      resp->cmd  = C_NOTE_REPLY;
      resp->addr = req->addr;
      resp ->U8_info =  ret;                
    }
    break;
					
    default:return;
  }
  send_rf_cmd_to_master(resp);

}

static void send_rf_cmd_to_master(pkt_master_t* resp)
{
  USART2_Send((uint8*)resp, sizeof(pkt_master_t));
}

void report_timeout_to_master(uint16 tagaddr)
{
  uint8 sendbuf[10] = {0};
  pkt_master_t* resp = (pkt_master_t*)sendbuf;
  resp->head = C_SYNC_HEAD;
  resp->tail = C_SYNC_TAIL;
  resp->cmd  = C_TMOUT_REPLY;
  resp->addr = tagaddr;    
  send_rf_cmd_to_master(resp);
}

void send_alive_cmd_to_master(void)
{
  uint8 sendbuf[10] ={0};
  pkt_master_t *resp = (pkt_master_t*)sendbuf;
  
  resp->head = C_SYNC_HEAD;
  resp->cmd  = 0x55;
  resp->addr = RfConfig.shortAddr;
  resp ->U8_info =  RfConfig.channel;
  resp ->U16_info = RfConfig.panId;
  resp->tail = C_SYNC_TAIL;
  send_rf_cmd_to_master(resp);
}

/////////////////////////////////////




