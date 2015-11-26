
#include "app_config.h"
#include "bsp.h"

uint16 test_mode_timeout = 0;

void tag_test_wait_key(KEY_TypeDef key)
{
	system_flag.key_press = 0;
	system_flag.key_protect = 0;
	
	test_mode_timeout = 10*120;
  while(1)
	{
		system_flag.key_press = 0;
  	system_flag.key_protect = 0;
		Delay100us(1000);
		if(key == 0xff && system_flag.key_press>0) break;
		if(key != 0xff)
		{
			if(system_flag.key_press == (0x01<<key)) break;
		}
		
		test_mode_timeout -= 1;
		if(system_flag.key_press == 0x05 || test_mode_timeout == 0) 
		{
			memset(backgroud, 0, 1024);
			item_show_msg("-QUIT--TEST-MODE", 16, 1, TRUE, 1);
			Delay100us(10000);
			key_reset_system();
		}
	}
}
static uint16 test_recv_times = 0;
static uint16 test_rssi_max = 0;
static uint16 test_rssi_min = 0;
void cale_send_and_recv(uint8 *pkt_tmp, uint8 pkt_len,uint8 rssi)
{
	if(memcmp(pkt_tmp+3,tag_mac_Addr, 8) == 0){
		Led_toggle(LED_GREEN);
		test_recv_times++;
		if(rssi<test_rssi_min) test_rssi_min = rssi;
		if(rssi>test_rssi_max) test_rssi_max = rssi;
	}
}


void tag_self_test(void)
{
	uint8 gui_data[16];
	uint8 led = 0;
	
	uint8 send_buf[12];
	uint16 send_times = 0;
	
	if(system_flag.key_press != 0x05) return;
	

	memset(backgroud, 0, 1024);
	
	memset(gui_data, 0, 16);
	sprintf(gui_data, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", tag_mac_Addr[7],
																								tag_mac_Addr[6],
																								tag_mac_Addr[5],
																								tag_mac_Addr[4],
																								tag_mac_Addr[3],
																								tag_mac_Addr[2],
																								tag_mac_Addr[1],
																								tag_mac_Addr[0]);
	
	item_show_msg("--TAG-TEST-MODE-", 16, 0, FALSE, 0);
	item_show_msg(gui_data,           16, 1, FALSE, 0);
	
	/*power manage test*/	
	App_GetEnergy();
	memset(gui_data, 0, 16);
	sprintf(gui_data, "POWER-%d.%.2dV", system_flag.sys_power/100, (system_flag.sys_power%100));
	item_show_msg(gui_data, strlen(gui_data), 3, TRUE, 1);
	
	tag_test_wait_key(KEY_PUT);
	
	memset(backgroud, 0, 1024);
  //item_show_msg("KEY-AND-LED-TEST", 16, 1, TRUE, 1);
	//tag_test_wait_key(KEY_PUT);
	turn_all_led_off();
	Set_LedOn(LED_WHITE);
	item_show_msg("KEY-PUT-------OK", 16, 1, TRUE, 1);

	tag_test_wait_key(KEY1);
	turn_all_led_off();
	Set_LedOn(LED_YELLOW);
	item_show_msg("KEY-1---------OK", 16, 1, TRUE, 1);
	
	tag_test_wait_key(KEY2);
	turn_all_led_off();
	Set_LedOn(LED_RED);
	item_show_msg("KEY-2---------OK", 16, 1, TRUE, 1);
	
	tag_test_wait_key(KEY3);
	turn_all_led_off();
	Set_LedOn(LED_GREEN);
	item_show_msg("KEY-3---------OK", 16, 1, TRUE, 1);
	
	memset(backgroud, 0, 1024);
	tag_test_wait_key(KEY_PUT);
	turn_all_led_off();
	
	/*Tag lowpower test*/

  memset(backgroud, 0, 1024);
  	
		item_show_msg("TAG-SLEEPING-NOW", 16, 1, TRUE, 1);
		SystemSleep_wakeBy_WorAndKey();
		item_show_msg("-TAG-RUNNING-NOW", 16, 1, TRUE, 1);
		tag_test_wait_key(KEY_PUT);
		item_show_msg("TAG-SENDING-----", 16, 1, TRUE, 1);
		
		A7139_SetChn(1);
		test_recv_times = 0;
		send_times = 500;
		test_rssi_max = 0;
		test_rssi_min = 0xff;
		
		while(send_times>0)
		{ send_buf[0] = 11;
	    send_buf[1] = 0xe3;
	    send_buf[2] = 0xe4;
	    memcpy(&send_buf[3], tag_mac_Addr, 8);
      send_buf[11] = 0xaa;
			A7139_Send_Msg(send_buf, 12);
			//Delay100us(10*50);
			Delay100us(10*10);
			send_times--;
		}
		
		memset(backgroud, 0, 1024);
		                        
		if(test_recv_times == 0) item_show_msg("RECV-------ERROR", 16, 1, TRUE, 1);
		else{										
		memset(gui_data, 0, 16);
	  sprintf(gui_data, "RECV-%.2d", test_recv_times/5);
	  item_show_msg(gui_data, strlen(gui_data), 1, FALSE, 0);
		
		memset(gui_data, 0, 16);
		sprintf(gui_data, "RSSI-%d-%d", test_rssi_min,test_rssi_max);
		item_show_msg(gui_data, strlen(gui_data), 2, TRUE, 0);
		}
		tag_test_wait_key(KEY_PUT);
		
		memset(backgroud, 0, 1024);
			item_show_msg("-QUIT--TEST-MODE", 16, 1, TRUE, 1);
			Delay100us(10000);
			key_reset_system();
		
	
	
}




