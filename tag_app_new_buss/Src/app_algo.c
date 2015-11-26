
#include "bsp.h"


/*系统状态信息*/
system_flag_t system_flag = {0};
/*标签MAC地址*/
uint8 tag_mac_Addr[8] = {0};


tag_flash_info_t tag_flash_info = {0};

app_state_para_t  state_para;

app_tag_state_t tag_state;

Mpdu_TxBuff_t TxMpdu; //for sender
Mpdu_RxBuff_t RxMpdu; //for receiver
BasicRfTxState_t TxState;


uint8 get_ledn_on(void);

void sys_info_print(void *s, int len)
{
	
#ifdef PRINT_DEBUG
	int i;
  uint8 turn[2] ={13,10};
  USART2_Send((uint8_t*)s, len);
	USART2_Send(turn, 2);
	USART2_Send(turn, 2);
#endif

}	



static void RfDataInit(void)
{
	uint8 i;
	memset(&TxMpdu,0,sizeof(Mpdu_TxBuff_t));
	memset(&RxMpdu,0,sizeof(Mpdu_RxBuff_t));
	for(i=0;i<MAX_MPDU_RXBUFF_NUM;i++)
	{
		RxMpdu.MpduBuff[i].pApduPayload = 0;
	}
	memset(&TxState,0,sizeof(BasicRfTxState_t));
	memset(&tag_state, 0, sizeof(app_tag_state_t));
}

uint8 get_random(uint8 min, uint8 max)
{
 
	return (min + (max-min)*(uint8)rand()/0xff);
}

void rf_para_clean(void)
{
	system_flag.rf_on_time = 0;
  system_flag.rf_pwr_time_sec = 0;
	
}

void system_Goto_sleep(void)
{
	if(!tag_state.sleepflag) return;
	
	/*keep awake when led on or rf RECV*/
	if(system_flag.led_on_time > 0 || system_flag.rf_on_time > 0) return;
										
	if(system_flag.led_pwr_time_sec == 0 || system_flag.rf_pwr_time_sec == 0) 
				system_flag.sys_pwr_time_sec = system_flag.led_pwr_time_sec + system_flag.rf_pwr_time_sec;
	else 
				system_flag.sys_pwr_time_sec = system_flag.led_pwr_time_sec>system_flag.rf_pwr_time_sec?system_flag.rf_pwr_time_sec:system_flag.led_pwr_time_sec;
	
	/*WOR sleep mode ? RTC sleep mode*/
	if(system_flag.sys_pwr_time_sec == 0){
		SystemSleep_wakeBy_WorAndKey();
	}else if(system_flag.rf_pwr_time_sec>0 || system_flag.led_pwr_time_sec>0)
	{
		uint8 msg_data[16];
		
		SystemSleep_s(system_flag.sys_pwr_time_sec); 
		
		memset(msg_data, 0, 16);
	  sprintf(msg_data, "sleep-%d",system_flag.sys_pwr_time_sec);
		sys_info_print(msg_data, strlen(msg_data));
	}
	/*if wakeup by key then do nothing*/
	if(system_flag.wakeup_except_rtc)
	{
		system_flag.wakeup_except_rtc = 0;
		sys_info_print("[wakeup bykey]", 14);
		return;
	}
	/*check RF RTC sleep timer*/
	if(system_flag.rf_pwr_time_sec >0)
	{
		system_flag.rf_pwr_time_sec  -= system_flag.sys_pwr_time_sec;
		
		if(system_flag.rf_pwr_time_sec == 0)
		{
			sys_info_print("wake to resend", 14);
			tag_state.sendflag = TRUE; //going to send a frame.
			tag_state.recvflag = FALSE;
		
		}
		
	}
	
	/*check RF RTC sleep timer*/
	if(system_flag.led_pwr_time_sec>0)
	{
		system_flag.led_pwr_time_sec -= system_flag.sys_pwr_time_sec;
		
		if(system_flag.led_pwr_time_sec == 0)
		{
			get_ledn_on();
		}
	}
	
}


static void App_data_Init(void)
{
    RfDataInit();
	
    tag_flash_info.short_addr   = DEFAULT_SHORTADDR;          //shortAddr
	  tag_flash_info.pan_id       = DEFAULT_PANID;              //panId
	  tag_flash_info.work_channel = DEFAULT_BUSS_CHN;
	  tag_flash_info.listen_channel = DEFAULT_BUGL_CHN;
    tag_state.token = ( ((uint16)(tag_mac_Addr[1]<<8)) |(tag_mac_Addr[0]) )
                 |\
                ( ((uint16)(tag_mac_Addr[3]<<8)) |(tag_mac_Addr[2]) &0xF7F7);
}



void AppRF_turn(TAG_STATE_MACHINE state)
{ 
  switch (state)
	{
		case NWK_FIND_INIT:
			A7139_SetChn(0x0);
			sys_info_print("[enter nwk channel]", 19);
			Enter_RX();
			break;
		case LISTEN_INIT:
			
			A7139_SetChn(tag_flash_info.listen_channel);
			sys_info_print("[enter listen channel]", 22);
		 
		  break;
		case STANDBY_INIT:
			A7139_SetChn(tag_flash_info.work_channel);
		  sys_info_print("[enter work channel]", 20);
			Enter_RX();
		default:break;
	}
	tag_state.sleepflag = FALSE;
	cancel_alarm_Interrupt();
}

void App_init_process(void)
{
	App_data_Init();
	
	/*读取系统记录信息*/
	Flash_ReadTagInfo();
	Flash_ReadTagState();
	
	/*clear qrcode and soft update flag*/
	if(tag_flash_info.qr_set_flag == SYS_PARA_MAGIC_NUM || tag_flash_info.update_software_flag == SYS_PARA_MAGIC_NUM) 
	{
		if(tag_flash_info.update_software_flag == SYS_PARA_MAGIC_NUM)
		{ 
			uint8 gui_data[16];	
			memset(backgroud, 0, 1024);
		  item_show_msg("SOFTWARE--UPDATE", 16, 1, FALSE, 1);
	    memset(gui_data, 0, 16);
	    sprintf(gui_data, "VERSION-%d", (uint8)APP_SOFT_VERSION);
			item_show_msg(gui_data,           16, 2, TRUE, 0);
			Delay100us(20000);
	  }
		tag_flash_info.qr_set_flag = 0;
		tag_flash_info.update_software_flag = 0;
		
		tag_flash_info.block_idx_flag_set = 0;
		
		Flash_SaveTagInfo();
	}
	
	/*标签状态机初始化*/
	if(SYS_PARA_MAGIC_NUM == tag_flash_info.net_flag_set)
	{
		tag_flash_info.pan_id   = tag_flash_info.pan_id;
		tag_flash_info.gateway_addr = tag_flash_info.gateway_addr;
		
		tag_state.state = NWK_JOIN_INIT;
		AppRF_turn(NWK_FIND_INIT);
	}
	else
	{
		tag_state.state = IDLE_RUN;
	}
	
	tag_state.cfg_sleeptime = tag_state.sleeptime = DEFAULT_SLEEPTIME;
	
	if(tag_flash_info.block_idx_flag_set == SYS_PARA_MAGIC_NUM)
	{
    update_screen_id(2);
	}
	
	//AppRF_turn(STANDBY_INIT);
	
	A7139_SetSleepTime(5);
	
}


void len_on_timer_handler(void)
{
	if(system_flag.led_on_time > 0)
  {
		system_flag.led_on_time --;
    if(system_flag.led_on_time == 0)
		{
			 turn_all_led_off();
			 system_flag.led_state += 0x10;
			 if(system_flag.led_state & 0x40)  system_flag.led_state &= 0x0f;
			 system_flag.led_pwr_time_sec = 1;//LED_OFF_SLEEP_TIME;//led off time
		}
	}		
}

void rf_on_timer_handler(void)
{
	if(system_flag.rf_on_time > 0)
  {
		system_flag.rf_on_time --;
    if(system_flag.rf_on_time == 0)
		{
			 system_flag.rf_pwr_time_sec = 1;
		}
	}		
}


void longdata_wait_timer_handler(void)
{
	if(system_flag.long_date_wait_time > 0)
  {
		system_flag.long_date_wait_time --;
	}	

}


void recv_data_reset_timer(void)
{
	tag_state.sleepflag = FALSE;
  system_flag.rf_on_time = 0;
}
void set_awake_time_after_send(uint32 ms)
{
	system_flag.rf_on_time = ms;
	
}
uint8 get_ledn_on(void)
{
	uint8 led_shift = (system_flag.led_state & 0x30)>>4;
	uint8 led_set   = system_flag.led_state & 0x0f;
	
	if(led_set == 0) return 1;
	
	while(!((0x1<<led_shift) & led_set))
	{
		led_shift +=0x1;
		if(led_shift & 0x04)  led_shift &= 0xf0;
	}
	
	system_flag.led_state = (system_flag.led_state & 0xcf) | (led_shift<<4);
	
	Set_LedOn((system_flag.led_state & 0x30)>>4);
	system_flag.led_on_time = 50;
	return 0;
}

/**key detect function**/

void Key_detect_Run(void)
{
	/*check if some key is press*/
	if(system_flag.key_press > 0) 
	{
		Delay100us(1000);
	}
  else return;
	
	system_flag.key_protect = 1;
	
	switch(system_flag.key_press)
	{
		case (0x01<<0):key_sync_func(KEY_PUT);
		break;
		case (0x01<<1):key_sync_func(KEY1);
		break;
		case (0x01<<2):key_sync_func(KEY2);
		break;
		case (0x01<<3):key_sync_func(KEY3);
		break;
		case ((0x01<<0) | (0x01<<1)):key_sync_func(KEY_PUT_1);
		break;
		case ((0x01<<0) | (0x01<<2)):key_sync_func(KEY_PUT_2);
		break;
		case ((0x01<<0) | (0x01<<3)):key_sync_func(KEY_PUT_3);
		break;
		case ((0x01<<1) | (0x01<<2)):key_sync_func(KEY1_2);
		break;
		case ((0x01<<1) | (0x01<<3)):key_sync_func(KEY1_3);
		break;
		case ((0x01<<2) | (0x01<<3)):key_sync_func(KEY2_3);
		break;
	}

		
	system_flag.key_press = 0;
	system_flag.key_protect = 0;
	
}


 void App_GetEnergy(void)
{
	Power_ADC_Init();
	
	/*After a power picking,fix pick cycle**/
	if(system_flag.sys_power <= FIX_CYCLE_VOLT) system_flag.power_detect_time = BUSY_PW_SAMPLE_CYCLE;
                                       else   system_flag.power_detect_time = LAZY_PW_SAMPLE_CYCLE;
        
  /*tag_state.energy is an presentation of Volt Percentage .**/
	tag_state.energy = 0x80|((uint8)((system_flag.sys_power/(float)MAX_VOLT)*100));
}


void App_Maintance(void)
{
	if((system_flag.sysTimeCount-system_flag.power_last_time)> system_flag.power_detect_time)
	{
		system_flag.power_last_time = system_flag.sysTimeCount;
		App_GetEnergy(); 
	}
}

/*
 *update software ready state and turn to iap to update
 */

void tag_app_update(void)
{uint8 data[50];
	Flash_ReadTagInfo();
	tag_flash_info.update_software_flag = SYS_PARA_MAGIC_NUM;
	tag_flash_info.software_crc =  cal_crc16((uint8*)STORE_APP_UPDATE_ADDR, 0x400*28);
	 
		sprintf(data, "crc-%.2x-%.2x", (uint8)(tag_flash_info.software_crc>>8),(uint8)(tag_flash_info.software_crc));
		sys_info_print(data, strlen(data));		
	Flash_SaveTagInfo();
	system_reset();
}

