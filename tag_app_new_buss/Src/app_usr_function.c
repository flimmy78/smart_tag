
#include "app_typedef.h"
#include "app_flash.h"
#include "app_wrscreen.h"
#include "app_usr_function.h"
#include "app_key.h"

uint8 curret_cur_offset = 1;
uint8 curret_cur_offset_max = 0;
uint32 InventoryMode_math_mi(uint8 n)
{
	uint32 data = 1;
	int i = 0;
	for(i = 0;i< n ;i++)
	{
		data *= 10;
	}
  return data;
}

void update_curret_cur_offset_max(void)
{
	uint32 num_save = state_para.pandian_num_block_num;

	curret_cur_offset_max = 0;
  do{
		num_save = num_save/10;
	  curret_cur_offset_max++;
	}while(num_save>0);
}
void update_screen_id(uint8 n)
{
  lite_screen_t inventorymode_screen_data;
	inventorymode_screen_data.charblockid = 0xFF;
	inventorymode_screen_data.dec_part = 0;
	inventorymode_screen_data.ers_flag = 0;
	inventorymode_screen_data.integ_part = 0;
	inventorymode_screen_data.numblockid = 0xff;
	inventorymode_screen_data.screen_id = n;
  inventorymode_screen_data.unit_show_type = 0;
	fill_disp_buffer((uint8*)&inventorymode_screen_data, backgroud, TRUE,FALSE, 0);
}

void update_InventoryMode_num(void)
{ 
	lite_screen_t inventorymode_screen_data;
	inventorymode_screen_data.charblockid = 0xFF;
	inventorymode_screen_data.dec_part = 0;
	inventorymode_screen_data.ers_flag = 0;
	inventorymode_screen_data.integ_part = state_para.pandian_num_block_num;
	inventorymode_screen_data.numblockid = state_para.pandian_num_block_id;
	inventorymode_screen_data.screen_id = 1;
  inventorymode_screen_data.unit_show_type = 0;
	fill_disp_buffer((uint8*)&inventorymode_screen_data, backgroud, TRUE, FALSE, curret_cur_offset);
}
void InventoryMode_Key_Plus(uint8 key)
{
	uint8 dig_num;
	dig_num= (state_para.pandian_num_block_num/InventoryMode_math_mi(curret_cur_offset - 1))%10;
	state_para.pandian_num_block_num -= dig_num*InventoryMode_math_mi(curret_cur_offset - 1);
  dig_num = (dig_num+1)%10;
	
	state_para.pandian_num_block_num += dig_num*InventoryMode_math_mi(curret_cur_offset - 1);
	
	if(dig_num == 0)
	{
		if(curret_cur_offset>curret_cur_offset_max) curret_cur_offset -- ;
		else if(curret_cur_offset == curret_cur_offset_max) {
			curret_cur_offset_max -- ;
			curret_cur_offset --;
		}
	}
	
	update_InventoryMode_num();
}

void InventoryMode_Key_minus(uint8 key)
{
	uint8 dig_num;
	dig_num= (state_para.pandian_num_block_num/InventoryMode_math_mi(curret_cur_offset - 1))%10;
	state_para.pandian_num_block_num -= dig_num*InventoryMode_math_mi(curret_cur_offset - 1);
  dig_num = (dig_num - 1 + 10)%10;
	
	state_para.pandian_num_block_num += dig_num*InventoryMode_math_mi(curret_cur_offset - 1);
	
	if(dig_num == 0)
	{
		if(curret_cur_offset>curret_cur_offset_max) curret_cur_offset -- ;
		else if(curret_cur_offset == curret_cur_offset_max) {
			curret_cur_offset_max -- ;
			curret_cur_offset --;
		}
	}
	
	update_InventoryMode_num();
}

void InventoryMode_Key_mode(uint8 key)
{
	curret_cur_offset +=1;	
	if(curret_cur_offset > curret_cur_offset_max + 1) curret_cur_offset = 1;
	if(InventoryMode_math_mi(curret_cur_offset- 1)>state_para.pandian_num_block_num) 
				state_para.pandian_num_block_num += InventoryMode_math_mi(curret_cur_offset - 1);

	
	update_InventoryMode_num();
}

void InventoryMode_Key_Confirm(uint8 key)
{
	Flash_SaveTagState();
  Usr_Event_Report(EVENT_INVENTORYMODE_CONFIRM);
	
	update_curret_cur_offset_max();
	tag_state.is_need_load_last_screen = 0;
	//app_func_InventoryMode_Quit();
	
	//update_screen_id(2);
}

void InventoryMode_Key_ReStart(void)
{
	state_para.pandian_num_block_num = 0;
	
	update_InventoryMode_num();
	
}


void app_func_InventoryMode_Enter(uint8 dig_block_id, uint32 dig_num)
{
  	back_up_InventoryMode_information();

		Flash_SaveTagState();
		
		key_state_change(KEY_PANDIAN);

		update_curret_cur_offset_max();	
}
void app_func_InventoryMode_Quit(void)
{
   	key_state_change(KEY_NORMAL);
	
	  back_up_InventoryMode_information(state_para.pandian_num_block_id,state_para.pandian_num_block_num);
}

///////////////////////////////////////////////////////////////////////////////////////////


void app_func_Normal_Enter(void)
{
		key_state_change(KEY_NORMAL);
}



/////////////////////
void active_screen_type_function(uint8 screen_id,uint8 dig_block_id, uint32 number)
{
	switch (screen_id)
	{
		case 1:
				app_func_InventoryMode_Enter(dig_block_id, number);
		break;
		default:
				app_func_Normal_Enter();
		break;
	}
}



void Usr_Event_Report(uint8 usr_event)
{
	tag_state.ackflag |= (1<<FLAG_TRANS_REPORT);
  tag_state.eve_counter[usr_event] ++;
	update_ack_info(tag_state.ackflag);
}


void usr_key_event_report(uint8 key)
{
	Usr_Event_Report(key + 0x10);
}





