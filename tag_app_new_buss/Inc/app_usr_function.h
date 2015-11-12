

#ifndef APP_USR_FUNC_H
#define APP_USR_FUNC_H

#include "app_config.h"
void InventoryMode_Key_Plus(uint8 key);
void InventoryMode_Key_minus(uint8 key);
void InventoryMode_Key_mode(uint8 key);
void InventoryMode_Key_Confirm(uint8 key);
void InventoryMode_Key_ReStart(void);
void Usr_Event_Report(uint8 usr_event);
void app_func_InventoryMode_Quit(void);
void usr_key_event_report(uint8 key);

#endif
