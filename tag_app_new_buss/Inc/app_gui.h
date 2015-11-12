#ifndef _APP_UTIL_H
#define _APP_UTIL_H

#include "bsp.h"

#define SCREEN_DATA_BUF_LEN   1024

extern uint8 backgroud[SCREEN_DATA_BUF_LEN];
extern uint16 req_slice ;
extern wr_screen_t screen;


void Write_GUI(uint8  page,uint8 col,uint8 wid,uint8 hig,uint8 *buf);
void backup_current_screen_context_data(uint8 screen_id);
void turn_gui_pages(uint8 key);

#endif
