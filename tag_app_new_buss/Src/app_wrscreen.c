/*
 * app_wrscreen.c
 */

#include "bsp.h"

cell_data_t c_d;
block_idx_info_t block_idx_info;

static uint8 last_refresh_char_block_id,last_refresh_dig_block_id;

static uint8 last_char_block,last_dig_block;
/*
  fill buffer,just display with block,note that matrix data is 
  fetch from external flash.SO:
  **in sys. initial, idx_info shall be read from flash.**
*/


void fill_disp_buffer(uint8* data_p,uint8 *back_p, bool save, bool screen_function_active, uint8 num_block_offset_black)
{
	uint8 idx = 0;
	uint8 cordx,cordy,temp;

	cell_data_t c_d;
	block_idx_info_t b_info;
	
	uint8 screen_id;
	uint8 char_block_id ;
	uint8 dig_block_id ;
	uint32 number ;
	uint8 dec;
      
	uint8 b_tmp;
            
	uint8 block_idx;
	
	
	plite_screen_t p_liteScreen = (plite_screen_t)(data_p);
        
	last_dig_block = last_char_block = 0xFF;
	
	if(!restore_screen_idx_tb_from_flash(&b_info)) return;
	
	if((p_liteScreen->charblockid == 0xFF) && (p_liteScreen->numblockid == 0xFF) && p_liteScreen->screen_id == 0xff) return;
	
	
	Init_EPaper(1);
      
	if(p_liteScreen->ers_flag) tag_state.erase_flag = TRUE;
												else tag_state.erase_flag = FALSE;
  screen_id = p_liteScreen->screen_id;
	char_block_id = p_liteScreen->charblockid;
	dig_block_id = p_liteScreen->numblockid;
	number = p_liteScreen->integ_part;
	dec	= p_liteScreen->dec_part;
	
	memset(backgroud,0, 1024);
  //显示页内容   
  Flash_ReadTagInfo();
	if(tag_flash_info.block_idx_flag_set == SYS_PARA_MAGIC_NUM && tag_flash_info.block_num > 0)
	{
		for(block_idx = 0; block_idx < b_info.block_num;block_idx ++) //cute_get_block_idx_guard
		{
		
			if((b_info.idx_tb[block_idx].screen_id & screen_id) && b_info.idx_tb[block_idx].blockid != dig_block_id)
			{
        
				Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
                      (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);

				Graph_Change(c_d.Coords_Y, c_d.Coords_X,
											c_d.Image_Width, c_d.Image_High,
												c_d.data, back_p, 0);
			}
		}
	 
	}
	 //使能当前屏幕功能
  	if(screen_function_active) active_screen_type_function(screen_id, dig_block_id, number);

	
	//显示文字块
	if(char_block_id != 0xFF)
	{
		
		for(block_idx = 0; block_idx < b_info.block_num;block_idx ++) //cute_get_block_idx_guard
		{
			if(b_info.idx_tb[block_idx].blockid == char_block_id)
			{
				last_char_block = char_block_id;
        
				Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
                      (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);

				Graph_Change(c_d.Coords_Y, c_d.Coords_X,
											c_d.Image_Width, c_d.Image_High,
												c_d.data, back_p, 0);
				break;
			}
		}
	}
      
	//显示数字块
	if(dig_block_id != 0xFF)
	{
		uint8 cnt = 0;
		
		//取得数字块坐标参数.
		for(block_idx = 0; block_idx < b_info.block_num;block_idx ++)
		{
			if((b_info.idx_tb[block_idx].blockid == dig_block_id))
			{
				last_dig_block = dig_block_id;

				Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
												(uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
								
				cordy = c_d.Coords_Y ;
				cordx = c_d.Coords_X ;
				break;
			}
		}
        
		if(p_liteScreen->unit_show_type == 1)//单位信息前面显示
		{
			b_tmp = UNIT_BLOCK_NO;
						
			for(block_idx = 0; block_idx < b_info.block_num;block_idx ++)
			{
				if(b_info.idx_tb[block_idx].blockid == b_tmp)///*显示单位*/
				{
					Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
											 (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
						
					Graph_Change(cordy, cordx,
												c_d.Image_Width, c_d.Image_High,
													c_d.data, back_p, 0);
									
					cordx += c_d.Image_Width + 1;
					break;
				}
			}
		}
        
		//显示数字整数部分
					
		while(number >= 0)
		{
			cnt++;
			number /= 10;
			if(!number) break;
		}
				
		Set_Block(cordy, cordx, c_d.Image_Width, c_d.Image_High, 0x00, back_p);
				
		idx = 0;
		number = p_liteScreen->integ_part;
		//wid_base = c_d[10].Image_Width;
				
		/*fill number block */
		b_tmp = NUM_BLOCK_BASE_NO;
       
		while(number >= 0)
		{
			temp = number%10;

			dig_block_id = temp + b_tmp;
			idx++;
						
			//read a digit matrix data from flash.
						
			for(block_idx = 0; block_idx <  b_info.block_num;block_idx ++)
			{
				if(b_info.idx_tb[block_idx].blockid == dig_block_id)
				{ 
					uint8 pandian_mode_black = 0;
					Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
											 (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
					 if(num_block_offset_black == idx && screen_id == 1) pandian_mode_black = 1;
					Graph_Change(cordy, cordx + (cnt-idx)*(c_d.Image_Width), //2 is dillimer bettewn digs.
												c_d.Image_Width, c_d.Image_High,
													c_d.data, back_p, pandian_mode_black);
					break;
				}
			}
						
			number /=10;
			if(!number){
				cordx += cnt*c_d.Image_Width +1;
				break;
			}
		}
        
		//显示小数部分
		if(dec)
		{
			b_tmp = DEC_BLOCK_NO;
						
			for(block_idx = 0; block_idx <  b_info.block_num;block_idx ++)
			{
				if(b_info.idx_tb[block_idx].blockid == b_tmp)///*显示小数点*/
				{  
					Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
											 (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
						
					Graph_Change(cordy, cordx, //2 is dillimer bettewn digs.
												c_d.Image_Width, c_d.Image_High,
													c_d.data, back_p, 0);
									
					cordx += c_d.Image_Width +1;
					break;
				}
			}


			b_tmp = NUM_BLOCK_BASE_NO;
				
			//显示小数数值部分
			while(dec)
			{
				temp = dec/10;
				dig_block_id = temp + b_tmp;
						
				//read a digit matrix data from flash.
						
				for(block_idx = 0; block_idx <  b_info.block_num;block_idx ++)
				{  
					if(b_info.idx_tb[block_idx].blockid == dig_block_id)
					{
						Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
											 (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
						
						Graph_Change(cordy, cordx,  
													c_d.Image_Width, c_d.Image_High,
														c_d.data, back_p, 0);
										
						cordx += c_d.Image_Width +1;
						break;
					}
				}
				
				dec %=10,dec *= 10;
				
				if(!dec) break;
			}
		}
      
		if(p_liteScreen->unit_show_type == 2)//后面显示
		{
			b_tmp = UNIT_BLOCK_NO;
						
			for(block_idx = 0; block_idx <  b_info.block_num;block_idx ++)
			{
				if(b_info.idx_tb[block_idx].blockid == b_tmp)///*显示小数点*/
				{  
					Flash_Read(b_info.idx_tb[block_idx].flash_page, b_info.idx_tb[block_idx].flash_offset,\
											 (uint8*)&c_d, b_info.idx_tb[block_idx].blocksize);
						
					Graph_Change(cordy, cordx,  
												c_d.Image_Width, c_d.Image_High,
													c_d.data, back_p, 0);
					break;
				}
			}
		}
	}
  

	//Write_EPaper_RAM(back_p,1);
      
	//update_disp_screen(); 
	refresh_screen(backgroud);
}

static void back_up_block_info_to_flash(uint8 *pdata,uint8 *pageindtor,uint8 blockidx,block_idx_info_t *b_info)
{   
	  cell_data_t c_d; 
	  ptemplate_t p_temp = (ptemplate_t)pdata;
    
    uint8 store_page = *pageindtor,priro_block;
    uint16 try_start_offset;
	
    if((pdata == NULL) ||(pageindtor == NULL)||(b_info == NULL)) return;
    
    
   
    
    if(blockidx < MAX_CELL_DATA)
    {
        c_d.blockid = p_temp->blockid;
			  c_d.screen_id = p_temp->screen_id;
        c_d.Coords_X =  p_temp->Coords_X;  
        c_d.Coords_Y = p_temp->Coords_Y;
        c_d.Image_Width = p_temp->Image_Width;
        c_d.Image_High = p_temp->Image_High;
        c_d.data_Lenth = p_temp->data_Lenth;
        c_d.displayflag = p_temp->displayflag;
        
        b_info->idx_tb[blockidx].blockid = p_temp->blockid;
        b_info->idx_tb[blockidx].screen_id = p_temp->screen_id;
        if(blockidx == 0) //第一块
        {
           Flash_Erase(store_page);
           b_info->idx_tb[blockidx].flash_page = store_page;
           b_info->idx_tb[blockidx].flash_offset = 0;
           b_info->idx_tb[blockidx].blocksize = p_temp->data_Lenth + LEN_EXCEPT_PAYLOAD_DATA;
        }
        else 
        {
           priro_block = (blockidx -1);
           try_start_offset = b_info->idx_tb[priro_block].flash_offset + b_info->idx_tb[priro_block].blocksize;
           
           if(try_start_offset%4) 
             try_start_offset += 4- try_start_offset%4; //紧挨上一块，字节对齐
           
           if( try_start_offset + p_temp->data_Lenth + LEN_EXCEPT_PAYLOAD_DATA > FLASH_PAGE_SIZE)
           {
             store_page++;
             Flash_Erase(store_page);
             try_start_offset = 0;
           }
           
           b_info->idx_tb[blockidx].flash_page = store_page;
           b_info->idx_tb[blockidx].flash_offset = try_start_offset;
           b_info->idx_tb[blockidx].blocksize = p_temp->data_Lenth+ LEN_EXCEPT_PAYLOAD_DATA;
        }

        memcpy(c_d.data,pdata + sizeof(template_t),p_temp->data_Lenth);   
      //  c_d.data = pdata + sizeof(template_t);
          
        //块信息写入flash区域
				 Flash_Write(STORE_BOOT_ADDR + b_info->idx_tb[blockidx].flash_page*1024 + b_info->idx_tb[blockidx].flash_offset, \
                      (uint8*)&c_d , (b_info->idx_tb[blockidx].blocksize));
				
        //Flash_Write(b_info->idx_tb[blockidx].flash_page*512 + \
         //             b_info->idx_tb[blockidx].flash_offset/4, \
         //             (uint8*)&c_d , (b_info->idx_tb[blockidx].blocksize)%4?(b_info->idx_tb[blockidx].blocksize)/4+1:(b_info->idx_tb[blockidx].blocksize)/4);
        
        *pageindtor = store_page; 
    }
}

void Write_EPaper(uint8 *data_p, uint8 *back_p)
{
  uint8 type = *(data_p);
  uint8 led = *(data_p+1);
  
  //WD_KICK();
  
  tag_state.buz_type = CMD_LONG_BUSSI_DATA;
  tag_state.buz_cmd_type = type;
  
  if(type == MATRIX_CMD)
  {
      uint8 i;
      uint8 *pdata = data_p + 3;
      uint8 *point = pdata + sizeof(template_t);
      uint8 pageindicator =(uint8)((STORE_BLOCK_GUI_PAGE_ADDR - STORE_BOOT_ADDR)/0x400);
      
      block_idx_info_t b_info;
      
      b_info.block_num  = *(data_p+2);
      
      //Init_EPaper(1); 
      memset(back_p,0,1024);//Set_Block(0,0,LCD_COL_MAX,LCD_LINE_MAX,0,back_p); 
      
      for(i=0;i<b_info.block_num ;i++)
      {
				ptemplate_t p_temp = (ptemplate_t)pdata;
            
        if(p_temp->displayflag == 1 && (p_temp->screen_id & 2))
				{
	         Graph_Change(p_temp->Coords_Y, p_temp->Coords_X, p_temp->Image_Width, p_temp->Image_High,
	     	                                                                      point, back_p, 0);   
        } 		      
						
        //模板数据生成idx表，保留至flash
        back_up_block_info_to_flash(pdata,&pageindicator,i,&b_info);
            
				pdata = point + p_temp->data_Lenth;
				point = pdata + sizeof(template_t);
      }
      
      backup_idx_info_to_flash(&b_info);
    // Write_EPaper_RAM(back_p,1);
     // update_disp_screen();
			update_screen_id(2);
  }
  else if(type == GUI_DATA)
  {
     uint8 gui_no = *(data_p+3);
     uint8 refreshflag = *(data_p+4);
     uint8 *pdata = data_p + 5;
     
     if(gui_no >=  USR_GUI_PAGE_SIZE) return;
		
    Flash_ReadTagState(); 
		if(gui_no == 0) back_up_gui_screen_num_to_flash(0);
		else if(state_para.gui_screen_set == SYS_PARA_MAGIC_NUM && gui_no > state_para.gui_screen_num) back_up_gui_screen_num_to_flash(gui_no);
		
     if(refreshflag && gui_no>0)
     {
			 
			 sys_info_print("DIS MSG:", 8);
        memcpy(back_p,pdata,1024);
         
       // Init_EPaper(1);
        //Write_EPaper_RAM(back_p,1);
        refresh_screen(backgroud);
        
        //backup_current_screen_context_data(10); 
     }
     
     store_usr_gui_to_flash(gui_no,pdata);
  }
  
   /*light led after refresh screen.*/
   tag_state.led.type = led;
   //turn_work_light_on(tag_state.led.type);
}


void refresh_screen(uint8 *back_p)
{
    Init_EPaper(1);
	 Clear_EPaper();
    Write_EPaper_RAM(back_p,1);
    update_disp_screen();
}


void Erase_Indic_block(uint8 *back_p)
{
      uint8 idx;
      cell_data_t c_d;
      block_idx_info_t b_info;
      
      if((last_char_block == 0xFF) &&(last_dig_block == 0xFF))
        return;
      
      if(!restore_screen_idx_tb_from_flash(&b_info))
        return;
      
      Init_EPaper(2);
      
      if(last_char_block != 0xFF)
      {
        for(idx = 0; idx < b_info.block_num;idx ++)
        {
              if(b_info.idx_tb[idx].blockid == last_char_block)//
              {
              
                 Flash_Read(b_info.idx_tb[idx].flash_page, b_info.idx_tb[idx].flash_offset,\
                     (uint8*)&c_d, b_info.idx_tb[idx].blocksize);
      
                 Set_Block( c_d.Coords_Y,
		      c_d.Coords_X,
		      c_d.Image_Width,
		      c_d.Image_High,
		      0x00,
		      back_p );
                break;
              }
         }
      }
      
      if(last_dig_block != 0xFF)
      {
        //display digit block,note this block area is to get its location factor.
        for(idx = 0; idx < b_info.block_num;idx ++)
        {    
            if(b_info.idx_tb[idx].blockid == last_dig_block)///*显示小数点*/
            {
                Flash_Read(b_info.idx_tb[idx].flash_page, b_info.idx_tb[idx].flash_offset,\
                      (uint8*)&c_d, b_info.idx_tb[idx].blocksize);
          
      
                Set_Block( c_d.Coords_Y,
		     c_d.Coords_X,
		     c_d.Image_Width,
		     c_d.Image_High,
		     0x00,
		     back_p );
                 break;
            }
        }
        Write_EPaper_RAM(back_p,1);
      }
      	
      update_disp_screen();
      backup_current_screen_context_data(10);
}
