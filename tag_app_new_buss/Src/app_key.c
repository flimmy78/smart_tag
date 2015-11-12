
#include "app_key.h"
#include "app_config.h"
#include "bsp.h"
#include "app_usr_function.h"
#include "app_gui.h"

static KEY_StateDef key_state = KEY_NORMAL;

void key_state_change(KEY_StateDef state)
{
  key_state = state;
}

void key_reset_system(uint8 key)
{
	system_reset();
}
void put_key_to_set_all_led_off(uint8 key)
{
  if(system_flag.led_state>0) {
		system_flag.led_state = 0x0;
		Usr_Event_Report(EVENT_KEY_PUT_PRESS);
	}
}

void key_sync_func(KEY_TypeDef key)
{
	KEY_FUNC key_function[10];
	uint8 key_n_t = 0;
	for(key_n_t = 0;key_n_t<10;key_n_t++) key_function[key_n_t].func_cmd = NULL;
	switch(key_state)
	{
		case KEY_NORMAL:
			key_function[KEY_PUT].func_cmd = put_key_to_set_all_led_off;
	    key_function[KEY2].func_cmd = turn_gui_pages;
      key_function[KEY_PUT_2].func_cmd = key_reset_system;
		  key_function[KEY_PUT_1].func_cmd = usr_key_event_report;
	   	key_function[KEY_PUT_3].func_cmd = usr_key_event_report;
		break;
		case KEY_WAIT:
	    key_function[KEY2].func_cmd = key_reset_system;
	    key_function[KEY3].func_cmd = key_reset_system;
		break;
		case KEY_PANDIAN:
			key_function[KEY_PUT].func_cmd = InventoryMode_Key_Confirm;
      key_function[KEY1].func_cmd = InventoryMode_Key_Plus;
	    key_function[KEY2].func_cmd = InventoryMode_Key_minus;
	    key_function[KEY3].func_cmd = InventoryMode_Key_mode;
		break;
		default:
			key_state = KEY_NORMAL;
			key_function[KEY2].func_cmd = key_reset_system;
	    key_function[KEY3].func_cmd = key_reset_system;
		break;
	}
	
  if(key_function[key].func_cmd != NULL) key_function[key].func_cmd(key);
}










