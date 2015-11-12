
#ifndef _APP_FLASH_H
#define _APP_FLASH_H
#include "bsp.h"

void Flash_SaveTagInfo(void);
void Flash_ReadTagInfo(void);

void Flash_Read(uint8 pg, uint16 offset, uint8 *buf, uint16 cnt);
void Flash_Write(uint32 addr, uint8 *buf, uint16 cnt);
void Flash_Erase(uint8 pg);

/*系统常驻信息更新*/
void update_find_NetWorkInfo(uint16 panid,uint16 gateway);
void update_join_NetWorkInfo(const uint8 listen_channel,const uint8 work_channel, const uint16 shortAddr);


/*更新反馈信息*/
void update_ack_info(uint16 data);

/* 系统GUI暂存页读取与存储 */
void read_backed_screen_context_from_flash(uint8* pdata,uint16 len);
void back_up_screen_context_to_flash(uint8* pdata,uint16 len);

/*用户GUI读取与存储*/
void load_usr_gui(uint8 gui_no);
void store_usr_gui_to_flash(uint8 gui_no, uint8 *pdata);

/*块刷屏GUI表读取与存储*/
bool restore_screen_idx_tb_from_flash(block_idx_info_t *pidx_table);
void backup_idx_info_to_flash(block_idx_info_t *pidx_table);

 

#endif
