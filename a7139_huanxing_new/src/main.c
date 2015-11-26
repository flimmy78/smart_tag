
#include "stm32f0xx.h"
#include "BSP_USART2.h"
#include "sleep_wake.h"
#include "main.h"

uint32_t sysTimeDelay = 0;
uint32   sysTime1ms = 0;

void SysTick_Init(void);
void RCC_Init(void);
void Delay_ms(uint32_t nMs);

extern void usart_msg_handle(void);
extern uint8 send_msg_buf[64];

uint8 tag_check_flag = 0;

void IWDG_Init();
void Feed_IWDG();

int main(void)
{
	SysTick_Init();

	RCC_Init();
  
	USART2_Init();
	
	InitRF();
	
	Tag_Init();
	
	//send_alive_cmd_to_master();
   
	IWDG_Init();
	
	//A7139_Fill_FIFO(&send_msg_buf[0], send_msg_buf[0] + 1 + 2);
	
   while(1)
   {			 
		if(sysTime1ms % 50 == 0) 
		{
		
			Feed_IWDG();
		}
		
     usart_msg_handle();
		 if(tag_check_flag)
		 {
			 Tag_state_check();
			 tag_check_flag = 0;
		 }
		 SendWakeMsg();
   }
}
/**************************************************/

void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
void Delay_ms(uint32_t nMs)
{
  sysTimeDelay = nMs;
	while(sysTimeDelay != 0x00);
}

void Delay10us(uint8 n)
{
  uint8 i;
	
	while(n--)
	{
        for(i=0; i<48; i++);
	}
}

void SysTick_Init(void)
{
  if(SysTick_Config(SystemCoreClock/1000))
	{
    while(1);
  }
}

uint16 uart_buff_clean_delay = 0;

void SysTime_handle(void)
{ 
	if(sysTimeDelay>0) sysTimeDelay--;
	sysTime1ms++;
	tag_timeout_handler();
	
	if(uart_buff_clean_delay>0)
	{
		uart_buff_clean_delay --;
		if(uart_buff_clean_delay == 0)
		{
      uart_buffer_clean(1);
		}

	}
}

void RCC_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
}

void IWDG_Init()
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(781);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void Feed_IWDG()
{
	IWDG_ReloadCounter();
}




