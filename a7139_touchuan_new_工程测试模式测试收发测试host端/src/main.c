
#include "stm32f0xx.h"
#include "BSP_USART2.h"
#include "main.h"


BasicRfCfg_t RfConfig = {0};

/////////////
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
uint32   sysTime_count = 0;
uint32_t sysTimeDelay = 0;
uint32   sysTime1ms = 0;
uint16 uart_buff_clean_delay = 0;



void IWDG_Init();
void Feed_IWDG();

void Delay_ms(uint32_t nMs)
{
  sysTimeDelay = nMs;
	while(sysTimeDelay != 0x00);
}

void Delay10us(uint8 n)
{
  uint8 i;
	uint8 k = (uint8)(SystemCoreClock/1000000);
	
	while(n--)
	{
        for(i=0; i<k; i++);
	}
}

void SysTick_Init(void)
{
  if(SysTick_Config(SystemCoreClock/1000))
	{
    while(1);
  }
}


void SysTime_handle(void)
{ 
	sysTime_count++;
	if(sysTimeDelay>0) sysTimeDelay--;
	
	sysTime1ms++;
	if(sysTime1ms%TAG_REPORT_LIVING_TIME == 0)
	{
		//send_alive_cmd_to_master();
	}
	
	if(uart_buff_clean_delay>0)
	{
		uart_buff_clean_delay --;
		if(uart_buff_clean_delay == 0)
		{
      uart_buffer_clean(1);
		}

	}
	
}

uint8 has_data_to_recv = 0;

int main(void)
{
	SysTick_Init();
	
	USART2_Init();

	InitRF();
	
	RfConfig.channel = DEFAULT_BUSS_CHN; //DEFAULT_JOIN_CHN;//DEFAULT_BUSS_CHN//DEFAULT_JOIN_CHN
	RfConfig.panId   = DEFAULT_PANID;
	RfConfig.shortAddr = DEFAULT_GW;
	//send_alive_cmd_to_master();
	
	A7139_SetChn(RfConfig.channel);
	
	IWDG_Init();
	
	while(1)
	{
		if(sysTime1ms % 50 == 0) 
		{
			Feed_IWDG();
		}
		if(has_data_to_recv) 
		{
			A7139_RX_Exti(DISABLE);
			//a7139_recv_handle();
			has_data_to_recv = 0;
			A7139_RX_Exti(ENABLE);
		}
		if(Data_from_Rf_buffer.pktcnt >0)
		{
			uint8 ops ;
			A7139_RX_Exti(DISABLE);
			ops = rf_get_usart_buffer_ops();
			USART2_Send(&Data_from_Rf_buffer.recvdata[ops][0], Data_from_Rf_buffer.recvdata[ops][0] + 1 + 2);
			Data_from_Rf_buffer.pktcnt -- ;
			A7139_RX_Exti(ENABLE);
		}
		
		if(Data_from_Uart_buffer.pktcnt > 0)
		{ 
			
			uint8 ops;
			
			ops = get_usart_buffer_ops();
			
			if(PKT_CONFIG_MAX_LEN + 1 == Data_from_Uart_buffer.recvdata[ops][0] &&  Data_from_Uart_buffer.recvdata[ops][1] == C_SYNC_HEAD
																																		      &&  Data_from_Uart_buffer.recvdata[ops][8] == C_SYNC_TAIL) 
			{	
				Model_Config((pkt_master_t*)&Data_from_Uart_buffer.recvdata[ops][1]);
				Data_from_Uart_buffer.pktcnt -- ;
				continue;
			}
			
			if(SUCCESS == A7139_Send_Msg(&Data_from_Uart_buffer.recvdata[ops][0], Data_from_Uart_buffer.recvdata[ops][0] + 1 + 2))
			{  
				Data_from_Uart_buffer.pktcnt -- ;
				continue;
      }
			
			
    }
	}
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




