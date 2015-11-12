
#include "bsp.h"



//À——∞Õ¯¬Á≥ı ºªØ
static void App_Find_Init(void)
{
	tag_flash_info.pan_id     = BROADCAST;  
	tag_flash_info.short_addr = BROADCAST;
	tag_flash_info.gateway_addr   = BROADCAST;
	   
  tag_state.sleepflag	= FALSE;
  tag_state.sendflag	= TRUE;

  tag_state.checkfactor = 0;

	tag_state.state	= NWK_FIND_RUN; 
}

//’˝‘⁄À——∞Õ¯¬Á
static void App_Find_Run(void)
{
	if(tag_state.sendflag)
	{
		if(tag_state.checkfactor == TIMEOUT_RETRY_FIND)
		{
			item_show_msg("---FAILED-JOIN--", 16, 2,TRUE, 1);
			tag_state.state = IDLE_RUN;
			tag_state.checkfactor = 0;
		}
		
		App_SendNetworkSearchMsg();//send for gateway found.
		tag_state.sendflag = FALSE;
		tag_state.checkfactor ++;
	}
}

//≥¢ ‘º”»ÎÕ¯¬Á≥ı ºªØ
static void App_Join_Init(void)
{  
  tag_state.checkfactor = 0;
      
	tag_state.sleepflag = FALSE;
	tag_state.sendflag = TRUE;
	
  tag_state.state = NWK_JOIN_RUN;
}

//≥¢ ‘º”»ÎÕ¯¬Á
static void App_Join_Run(void)
{
	if(tag_state.sendflag)
	{
		if(tag_state.checkfactor == TIMEOUT_JOIN_WAIT)
		{
			item_show_msg("---FAILED-JOIN--", 16, 2,TRUE, 1);
			tag_state.state = IDLE_RUN;
			tag_state.checkfactor = 0;
		}
		App_SendJoinRequestMsg();
		tag_state.sendflag = FALSE;
		tag_state.checkfactor ++;
	}
	
  if(tag_state.state == STANDBY_INIT) 
	{   
		req_slice = 0; //for first request.
	}
}


static void App_Listen_Init(void)
{
  tag_state.state = LISTEN_RUN;
}

static void App_Listen_Run(void)
{
	tag_state.sleepflag  = TRUE;
	if((tag_state.ackflag & 0x7fff) ||(req_slice))
  {
		tag_state.sleepflag  = FALSE;
		tag_state.state = STANDBY_INIT;
		AppRF_turn(STANDBY_INIT);
  }
	if(system_flag.sys_wake_up)
	{
		sys_info_print("wake up from listen...", 22);
		tag_state.sleepflag  = FALSE;		
		tag_state.state = STANDBY_INIT;
		AppRF_turn(STANDBY_INIT);
		system_flag.sys_wor_sleep = 0;
		system_flag.sys_wake_up = 0;
		test_recv_time(0);
		tag_state.is_need_load_last_screen = 1;
	}
}

static void App_Standby_Init(void)
{
	sys_info_print("App_Standby_Init...", 19);
	memset(&RxMpdu,0,sizeof(Mpdu_RxBuff_t));
	
	tag_state.sendflag = TRUE;
	tag_state.recvflag = FALSE;    
	tag_state.sleepflag = FALSE;
  
	tag_state.state = STANDBY_RUN;
	tag_state.checkfactor = 0;
	
}



static void App_Standby_Run(void)
{ 
	 //check if tag has message to ack.
	if(tag_state.sendflag) 
	{
    if(tag_state.checkfactor == TIMEOUT_RETRY_BUSS)
		{
			tag_state.state = LISTEN_INIT;
		  AppRF_turn(LISTEN_INIT);
		  rf_para_clean();
			tag_state.checkfactor = 0;
		}
		
    if(tag_state.ackflag & 0x7fff){
			
			uint16  tmp = tag_state.ackflag;
			uint8 idx = 0;
                    
       /*extract which event to be acked*/
       while((!(tmp  & 0x0001))&&(idx<15))
       {
          idx++;               
          tmp >>= 1;
       }
                 
			App_SendEventRspMsg(idx); //report exec. status to gw
    }else if(req_slice){
      App_SendRetransReqMsg();
    }else{
			App_SendQueryMsg();//use 7ms test
			
    }
		
    tag_state.sendflag = FALSE;
		tag_state.checkfactor ++;
	}

}

static void App_Write_Init(void)
{       
	sys_info_print("App_WRITE_Init...", 17);
	tag_state.recvflag = FALSE;
  tag_state.sleepflag = FALSE;
	
	tag_state.state = WRITE_RUN;
  
	system_flag.long_date_wait_time = 5000;
	
}



static void App_Write_Run(void)
{
		
 if(0 == system_flag.long_date_wait_time)
 {
		tag_state.state = STANDBY_INIT;
 }
}

static void App_Idle_Run(void)
{ 
	SystemSleep_s(0);
	item_show_msg("---REFIND-NET---", 16, 2,TRUE, 1);
	tag_state.state = NWK_FIND_INIT;
	AppRF_turn(NWK_FIND_INIT);
}

static void App_Hold_For_Next_Find()
{
    
  tag_state.state = NWK_FIND_INIT;      
}

static void App_Night_Mode_Turn()
{
     //App_night_mode_process();
}


void AppState_Run(void)
{
	switch(tag_state.state)
	{
	case NWK_FIND_INIT:
	  App_Find_Init();
		break;
	case NWK_FIND_RUN:
		App_Find_Run();
		break;
	case NWK_JOIN_INIT:
		App_Join_Init();
		break;
	case NWK_JOIN_RUN:
		App_Join_Run();
		break;
	case LISTEN_INIT:
		App_Listen_Init();
		break;
	case LISTEN_RUN:
		App_Listen_Run();
		break;
	case STANDBY_INIT:
		App_Standby_Init();
		break;
	case STANDBY_RUN:
		App_Standby_Run();
		break;
	case WRITE_INIT:
		App_Write_Init();
		break;
	case WRITE_RUN:
		App_Write_Run();
		break;
	case IDLE_RUN:
    App_Idle_Run();
    break;
  case HOLD_NEXT_FIND:
   	App_Hold_For_Next_Find();
    break;
  case NIGHT_MODE:
   	App_Night_Mode_Turn();
    break;
	default:
		break;
	}
}

void tag_app_sync_pkt(void)
{
	uint8 state_save = tag_state.state;
	if(RxMpdu.MpduNum == 0) return;
	
	recv_data_reset_timer();
	sys_info_print("Recv MSG:", 8);
	
	
  switch(tag_state.state){
		case NWK_FIND_RUN:
			App_RecvNetworkSearchMsgRsp();
		break;
		case NWK_JOIN_RUN:
			App_RecvJoinRequestMsgRsp();
		break;
		case STANDBY_RUN:
		{
			if(App_RecvStandByMsgRsp()) return;
		}
		break;
		case WRITE_RUN:
			App_Write_RecvProcess();
		break;
		default:break;
	}
	
	RxMpdu.CurrentMpdu++;
	RxMpdu.MpduNum--; 
	
	if(RxMpdu.CurrentMpdu==MAX_MPDU_RXBUFF_NUM)
	{
		RxMpdu.CurrentMpdu = 0;
	}
	if(state_save == tag_state.state && tag_state.state != WRITE_RUN)
	{
		tag_state.sleepflag = TRUE;
		system_flag.rf_on_time = 100;
	}
	
}

