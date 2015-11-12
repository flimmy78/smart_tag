

#ifndef _APP_ALGO_H
#define _APP_ALGO_H

#include "app_typedef.h"
#include "app_config.h"

extern uint8 tag_mac_Addr[8];



extern tag_flash_info_t tag_flash_info ;
extern  app_state_para_t  state_para;

extern Mpdu_TxBuff_t TxMpdu; //for sender
extern Mpdu_RxBuff_t RxMpdu; //for receiver
extern BasicRfTxState_t TxState;


extern app_tag_state_t tag_state;
extern BasicRfTxState_t TxState;

void system_Goto_sleep(void);
void App_init_process(void);
void App_night_mode_process(void);
void Turn_chanle(uint16 chan);

void App_init_process(void);

#endif



