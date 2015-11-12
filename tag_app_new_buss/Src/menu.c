
#include "bsp.h"
#include "menu.h"

void display_qrcode(void);

void item_show_msg(uint8 * __restrict data, uint8 len, uint8 Y, bool display, uint8 black)
{
	uint8 t = 0;
	uint8 buff[16] = {0};
	uint8 zimu[26][16] = {0};
  uint8 number[10][16] = {0};
  uint8 fuhao[2][16] = {0};

  memcpy(buff, data, len);
	memcpy(&number[0][0], (void*)(STORE_STATIC_NUM_GUI_ADDR), 16*10);
	memcpy(&zimu[0][0], (void*)(STORE_STATIC_ABC_GUI_ADDR), 16*26);
  memcpy(&fuhao[0][0], (void*)(STORE_STATIC_FUHAO_GUI_ADDR), 16*2);

  for(t = 0;t<len;t++)
  {
		uint8 pic_data[16];
		if(buff[t] >= (0x41 + 0x20) && buff[t] <= (0x5a + 0x20)) buff[t] -= 0x20;
		if(buff[t] >= 0x41 && buff[t] <= 0x5a) memcpy(pic_data, &zimu[buff[t]- 0x41][0], 16);
		else if (buff[t] >= 0x30 && buff[t] <= 0x39) memcpy(pic_data, &number[buff[t]- 0x30][0], 16);
		else if(buff[t] == 0x2d || buff[t] == 0x2e) memcpy(pic_data, &fuhao[buff[t]- 0x2d][0], 16);
		else continue;
	  Graph_Change(16*Y, t*8, 8, 16, pic_data, backgroud, black);
	}
  
	if(display) Write_GUI( 0,0,128,64,backgroud);
}

#ifdef TEST_TAG
void tag_test_print(uint16 sleep_time)
{
	uint8 gui_data[16];
	
	memset(backgroud, 0, 1024);
	
	
	sprintf(gui_data, "TIME-%d",sleep_time);
	item_show_msg(gui_data, 16, 0, FALSE, 1);
	
	sprintf(gui_data, "COST-%d",get_test_recv_time());
	item_show_msg(gui_data, 16, 1, FALSE, 1);
	
	item_show_msg("TEST-NEW-1-1", 12, 3, TRUE, 0);
}
#endif
void tag_self_test(void)
{
	uint8 gui_data[20];
	uint8 led = 0;
	
	if(system_flag.key_press != 0x05) return;
	system_flag.key_protect = 1;
	
	memset(backgroud, 0, 1024);
	
	memset(gui_data, 0, 20);
	sprintf(gui_data, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", tag_mac_Addr[7],
																								tag_mac_Addr[6],
																								tag_mac_Addr[5],
																								tag_mac_Addr[4],
																								tag_mac_Addr[3],
																								tag_mac_Addr[2],
																								tag_mac_Addr[1],
																								tag_mac_Addr[0]);
	
	item_show_msg("-MAC-", 5, 1, FALSE, 0);
	item_show_msg(gui_data, 16, 2, FALSE, 1);
	
	/*power manage test*/	
	App_GetEnergy();
	
	memset(gui_data, 0, 20);
	sprintf(gui_data, "POWER-%d.%.2dV", system_flag.sys_power/100, (system_flag.sys_power%100));
	item_show_msg(gui_data, strlen(gui_data), 3, TRUE, 0);
	
	for(led = 0;led<5;led++)
	{
		Set_LedOn(led);
		Delay100us(10000);
		turn_all_led_off();
	}
	
	system_flag.key_press = 0;
	system_flag.key_protect = 0;
	
	while(system_flag.key_press == 0)
	{
		Delay100us(1000);
	}
	system_flag.key_press = 0;
	system_flag.key_protect = 0;
}


void display_sleep_time(void)
{
  uint8 msg_data[20];
	memset(msg_data, 0, 20);
	sprintf(msg_data, "TIME-%d",tag_state.sleeptime);

	item_show_msg(msg_data, strlen(msg_data), 0, FALSE, 1);
}


void test_recv_time_dis(void)
{
	uint8 msg_data[20];
	
	memset(msg_data, 0, 20);
	sprintf(msg_data, "TIME-%d",tag_state.sleeptime);
	item_show_msg(msg_data, strlen(msg_data), 0, FALSE, 1);
	
  item_show_msg("TEST-1-2", 8, 3, TRUE, 1);
}

uint32 test_ms[2]={0};
void test_recv_time(int nu)
{
	if(nu == 0) test_ms[0] = system_flag.sysTimeCount;
	else test_ms[1] = system_flag.sysTimeCount - test_ms[0];
}

uint16 get_test_recv_time()
{
	if(test_ms[1] > 0xffff) return 0xffff;
	else return test_ms[1];

}


void display_qrcode(void)
{
	uint8 data_msg[16] ={0};
	if(tag_flash_info.qr_crc != cal_crc16(&tag_flash_info.qrcode[0], 63)) 
	{
		uint8 data[16];
		sprintf(data, "QR-ERROR");
		item_show_msg(data, 8, 3,TRUE, 1);
		return;
	}
	sprintf(data_msg, "VER-%.4d",APP_SOFT_VERSION);
	item_show_msg(data_msg, 8, 2,FALSE, 0);
	Graph_Change( 30, 97, 21, 24, tag_flash_info.qrcode, backgroud, 0);		
	//Write_GUI( 0,0,128,64,backgroud);
}

