
#include "bsp.h"

#include "gui_data.h"

wr_screen_t screen;
uint16 req_slice = 0;

uint8 backgroud[SCREEN_DATA_BUF_LEN]={0};

void gui_backgroud_clean(void)
{
	memset(backgroud, 0, 1024);
}
uint32 sum_check_cal32(uint8 *buf, uint16 len)
{
	uint16 i;
	uint32 sum_result = 0;
	
	for (i=0; i<len; i++) 
	{
		sum_result += ((uint8 *)buf)[i];
	}	
	
	return sum_result;
}



/*System gui function*/
void Write_GUI(uint8  page,uint8 col,uint8 wid,uint8 hig,uint8 *buf)
{
    Init_EPaper(1);
    Clear_EPaper();
    Write_EPaper_GUI(page,col,wid,hig,buf); 
}


void load_tag_logo(bool display)
{
	 memcpy(backgroud, (void*)(STORE_STATIC_GUI_ADDR), 1024);
	 if(display) Write_GUI( 0,0,128,64,backgroud);
}

void dis_play_state(TAG_STATEType gui, bool display)
{
	uint8 han_zi[10][32] = {0};
  uint8 wenzi_order[5];
  int t = 0;

	memcpy(&han_zi[0][0], (void*)(STORE_STATIC_ZIMO_GUI_ADDR), 32*10);
  //memset(backgroud, 0, 1024);
  switch (gui)
	{
		case SYS_GUI_NOT_IN_NET:
		{
			uint8 order[5] = {0,1,2,3,4};
			memcpy(wenzi_order, order, 5);
    }
		break;
		case SYS_GUI_FINDING_IN_NET:
		{
			uint8 order[5] = {0,1,3,4,5};
			memcpy(wenzi_order, order, 5);
    }
		break;
		case SYS_GUI_IN_NET:
		{
			uint8 order[5] = {0,1,6,3,4};
			memcpy(wenzi_order, order, 5);
    }
		break;
		

	}
	
	for(t = 0; t<5; t++)
	{
		Graph_Change(45, 24+t*16, 16, 16, han_zi[wenzi_order[t]], backgroud, 1); 
	} 
	
	if(display) Write_GUI( 0,0,128,64,backgroud);
}


void restore_screen_context_data()
{
    read_backed_screen_context_from_flash(backgroud,SCREEN_DATA_BUF_LEN);
}

void backup_current_screen_context_data(uint8 screen_id)
{
    back_up_screen_context_to_flash(backgroud,SCREEN_DATA_BUF_LEN);
	  
	  Flash_ReadTagInfo();
	  if(tag_flash_info.last_gui_flag_set != SYS_PARA_MAGIC_NUM || tag_flash_info.last_gui_screen_id != screen_id)
		{
			tag_flash_info.last_gui_flag_set = SYS_PARA_MAGIC_NUM;
			tag_flash_info.last_gui_screen_id = screen_id;
			Flash_SaveTagInfo();
		}
}


void refresh_screen_by_code(uint8 *pdata)
{
	
    fill_disp_buffer(pdata, backgroud, TRUE, TRUE,0);
}

void screen_turn_to_default(void)
{
	if(!tag_state.is_need_load_last_screen) return;
  if(tag_flash_info.last_gui_flag_set == SYS_PARA_MAGIC_NUM) 
	{
		restore_screen_context_data();
		return;
	}
	load_tag_logo(TRUE);
}





void turn_gui_pages(uint8 key)
{
	Flash_ReadTagState();
	if(state_para.gui_screen_set != SYS_PARA_MAGIC_NUM) return;
	state_para.current_gui_page = (state_para.current_gui_page+1)%(state_para.gui_screen_num+1);
	if(state_para.current_gui_page == 0 && state_para.gui_screen_num>0) state_para.current_gui_page = 1;
	Flash_SaveTagState();
	load_usr_gui(state_para.current_gui_page);
}



