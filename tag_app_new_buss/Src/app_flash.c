
#include "bsp.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 FLASH_Status Flash_WriteData(uint32 Address, uint16* Data, int size)
{
	uint16_t* data = Data;
	int data_ops = 0;
	
	for(data_ops = 0; data_ops < size; data_ops++)
	{
    if(FLASH_COMPLETE != FLASH_ProgramHalfWord(Address, *data))
		{
			return FLASH_TIMEOUT;
    }
		data++;
		Address+=2;
	}
	
	if(0 == memcmp((void*)Address, Data, size))
	{
    return FLASH_COMPLETE;
  }
	else
	{
		return FLASH_TIMEOUT;
  }
}

////


void Flash_SaveTagInfo(void)
{
	uint8 buff[1024] = {0};

  FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	if(FLASH_COMPLETE != FLASH_ErasePage(STORE_SYS_INFO_PAGE_ADDR))
	{
    return;
  }
	memcpy(buff, (uint8*)&tag_flash_info.net_flag_set, sizeof(tag_flash_info));
	
  Flash_WriteData(STORE_SYS_INFO_PAGE_ADDR, (uint16*)buff, (sizeof(tag_flash_info)+1)/2);

	FLASH_Lock();
}

void Flash_ReadTagInfo(void)
{
	memcpy((uint8*)&tag_flash_info.net_flag_set, (void*)STORE_SYS_INFO_PAGE_ADDR, sizeof(tag_flash_info));
}

void Flash_SaveTagState(void)
{
	uint8 buff[64] = {0};

  FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	if(FLASH_COMPLETE != FLASH_ErasePage(STORE_SYS_STATE_PAGE_ADDR))
	{
    return;
  }
	memcpy(buff, (uint8*)&state_para.ackflag_set, sizeof(state_para));
	
  Flash_WriteData(STORE_SYS_STATE_PAGE_ADDR, (uint16*)buff, (sizeof(state_para)+1)/2);

	FLASH_Lock();
}

void Flash_ReadTagState(void)
{
	memcpy((uint8*)&state_para.ackflag_set, (void*)STORE_SYS_STATE_PAGE_ADDR, sizeof(state_para));
}


void Flash_Read(uint8 pg, uint16 offset, uint8 *buf, uint16 cnt)
{
	uint32 pg_addr = 0x08000000 + ((uint32)0x400)*pg;
	memcpy(buf, (void*)(pg_addr + offset), cnt);
} 

void Flash_Write(uint32 addr, uint8 *buf, uint16 cnt)
{
	FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	
	Flash_WriteData(addr, (uint16_t*)buf, (cnt+1)/2);
	
	FLASH_Lock();
	
}

void Flash_Erase(uint8 pg)
{
	uint32 pg_addr = 0x08000000 + ((uint32)0x400)*pg;
	FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	if(FLASH_COMPLETE != FLASH_ErasePage(pg_addr))
	{
    return;
  }

	FLASH_Lock();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*************net para update*****************/
void update_find_NetWorkInfo(uint16 panid,uint16 gateway)
{  
	Flash_ReadTagInfo();
  
	tag_flash_info.net_flag_set = SYS_PARA_MAGIC_NUM;
	tag_flash_info.pan_id = panid;
	tag_flash_info.gateway_addr = gateway;
	
	Flash_SaveTagInfo();
}

void update_join_NetWorkInfo(const uint8 listen_channel,const uint8 work_channel, const uint16 shortAddr)
{
	Flash_ReadTagInfo();

	tag_flash_info.listen_channel = listen_channel;
	tag_flash_info.work_channel = work_channel;
	tag_flash_info.short_addr = shortAddr;
	
	Flash_SaveTagInfo();
}

/*************block idx update*****************/
bool restore_screen_idx_tb_from_flash(block_idx_info_t *pidx_table)
{
  if(pidx_table == NULL) return 0;
	Flash_ReadTagInfo();
  memcpy((uint8*)pidx_table,  (void*)&tag_flash_info.block_idx_flag_set,  sizeof(block_idx_info_t));
  return (pidx_table->magic_num == SYS_PARA_MAGIC_NUM);
}

void backup_idx_info_to_flash(block_idx_info_t *pidx_table)
{   
	Flash_ReadTagInfo();
  pidx_table->magic_num = SYS_PARA_MAGIC_NUM;
	memcpy((uint8*)&tag_flash_info.block_idx_flag_set,  (uint8*)pidx_table,  sizeof(block_idx_info_t));
	
	Flash_SaveTagInfo();
	
}

/*************state para update*****************/
void update_ack_info(uint16 data)
{
   Flash_ReadTagState();
	
   state_para.ackflag_set = SYS_PARA_MAGIC_NUM;
   state_para.cp_of_ackflag = data;
	
   Flash_SaveTagState();
}

void update_led_spark_paras(led_desc *pled)
{
    if(pled == NULL)
        return ;
		Flash_ReadTagState();
		
		state_para.led_set = SYS_PARA_MAGIC_NUM;
		memcpy((uint8*)&state_para.led,(uint8*)pled,sizeof(led_desc));
		
	Flash_SaveTagState();
}

void back_up_sleeptime_to_flash(uint32 sleeptime)
{
    Flash_ReadTagState();
	
    state_para.sleeptime_set = SYS_PARA_MAGIC_NUM;
    state_para.sleeptime = sleeptime;
    
   Flash_SaveTagState();
}

void back_up_reportflag_to_flash(uint8 flag)
{
   Flash_ReadTagState();
    
    state_para.reportflag_set = SYS_PARA_MAGIC_NUM;
    state_para.reportflag = flag;
    
    Flash_SaveTagState();
}

void back_up_InventoryMode_information(uint8 dig_block_id, uint32 dig_num)
{
	Flash_ReadTagState();
    
  state_para.pandian_num_block_id = dig_block_id;
  state_para.pandian_num_block_num = dig_num;
    
  Flash_SaveTagState();
}

void back_up_gui_screen_num_to_flash(uint8 gui_no_max)
{
   Flash_ReadTagState();
    
    state_para.gui_screen_set = SYS_PARA_MAGIC_NUM;
    state_para.gui_screen_num = gui_no_max;
    state_para.current_gui_page = 0xff;
    Flash_SaveTagState();
}

/*************GUI flash function*****************/



void Flash_SaveLastGUI(void)
{
	uint8 buff[1024] = {0};

  FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	if(FLASH_COMPLETE != FLASH_ErasePage(STORE_LAST_GUI_PAGE_ADDR))
	{
    return;
  }
	memcpy(buff, (uint8*)&backgroud, 1024);
	
  Flash_WriteData(STORE_LAST_GUI_PAGE_ADDR, (uint16*)buff, 1024/2);

	FLASH_Lock();
}

void store_gui_page(uint32 addr, uint8 offset, uint8* data)
{
	uint8 buff[1024] = {0};

  FLASH_Unlock(); 
	
	
  FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
	if(FLASH_COMPLETE != FLASH_ErasePage(addr))
	{
    return;
  }
	memcpy(buff, data, 1024);
	
  Flash_WriteData((uint32)(addr + offset*0x400), (uint16_t*)buff, (0x400 + 1)/2);
	FLASH_Lock();
}


void read_backed_screen_context_from_flash(uint8* pdata,uint16 len)
{ 
	memcpy(pdata, (void*)STORE_LAST_GUI_PAGE_ADDR, len);
	refresh_screen(pdata);
}

void back_up_screen_context_to_flash(uint8* pdata,uint16 len)
{
    //store_gui_page(STORE_LAST_GUI_PAGE_ADDR, 0, pdata);
	Flash_SaveLastGUI();
}
/*************USR GUI update*****************/
void load_usr_gui(uint8 gui_no)
{
  Flash_ReadTagState();
	if(state_para.gui_screen_set != SYS_PARA_MAGIC_NUM) return;
	if(gui_no>state_para.gui_screen_num) return;
	
  memcpy(backgroud, (void*)(STORE_USR_GUI_PAGE_ADDR + gui_no*1024), 1024);
  Write_GUI( 0,0,128,64,backgroud);
	tag_state.is_need_load_last_screen = 0;
	
	if(gui_no == 0)
	{
		state_para.gui_screen_num = 0;
		Flash_SaveTagState();
	}
  //backup_current_screen_context_data();
}

void store_usr_gui_to_flash(uint8 gui_no, uint8 *pdata)
{
	uint8 *data = pdata;
  store_gui_page(STORE_USR_GUI_PAGE_ADDR + gui_no*1024, 0, data); 
}















