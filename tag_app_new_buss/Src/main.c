//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================

#include "bsp.h"

extern void AppState_Run(void);
extern void tag_app_sync_pkt(void);
extern void App_Maintance(void);
extern void Key_detect_Run(void);


int main(void)
{
#ifdef PRINT_DEBUG
	USART2_Init();
#endif
	Board_Init();
	
	load_tag_logo(TRUE);
	Delay100us(10000);
	tag_self_test();
	
	key_state_change(KEY_NORMAL);
  App_init_process();

  while(1)
  {
		/*key detect*/
		Key_detect_Run();
		/*RX pkg sync */
		tag_app_sync_pkt();
    /*app state */
    AppState_Run();
		/*sleep and wake function*/
		system_Goto_sleep();
		/*power detect*/
		App_Maintance();
  }
}




