
#include "main.h"
#include "sleep_wake.h"

Data_from_Uart_to_Rf_t Data_from_Uart_buffer ={0};

////////////////////////////

void data_into_buffer(uint8* data)
{
  if( DATA_FROM_USART_BUFFSIZE == Data_from_Uart_buffer.pktcnt) 
    return;
  
  memcpy(&Data_from_Uart_buffer.recvdata[Data_from_Uart_buffer.currentData][0], data, PKT_MAX_LEN);
  
  Data_from_Uart_buffer.currentData++;
  Data_from_Uart_buffer.pktcnt ++;
  
  if( DATA_FROM_USART_BUFFSIZE == Data_from_Uart_buffer.currentData) Data_from_Uart_buffer.currentData = 0;
}

uint8 get_usart_buffer_ops()
{
  uint8 data_oldest = 0;
  if(Data_from_Uart_buffer.currentData >= Data_from_Uart_buffer.pktcnt) data_oldest = Data_from_Uart_buffer.currentData - Data_from_Uart_buffer.pktcnt;
  else  data_oldest = Data_from_Uart_buffer.currentData + DATA_FROM_USART_BUFFSIZE - Data_from_Uart_buffer.pktcnt;
  return data_oldest;
}

uint8 uart_recv_temp[PKT_MAX_LEN] = {0};
uint8 uart_recv_count = 0;

extern uint16 uart_buff_clean_delay;
void uart_buffer_clean(uint8 set)
{
	if(set == 0){
		uart_buff_clean_delay = 50;
	}else if(set == 1){
		//memset(uart_recv_temp, 0, PKT_MAX_LEN);
		uart_recv_count = 0;
	}else{
		uart_buff_clean_delay = 0;
	}
}

void usart_data_handle(uint8 data)
{
	
	if(C_SYNC_HEAD == data && uart_recv_count == 0)
  {
		uart_recv_count  = 1;
		uart_recv_temp[0] = data;
		uart_buffer_clean(0);
		return;
  }else if (C_SYNC_TAIL == data && uart_recv_count == PKT_MAX_LEN - 1)
	{
		uart_buffer_clean(2);
		uart_recv_temp[PKT_MAX_LEN - 1] = C_SYNC_TAIL;
		data_into_buffer(uart_recv_temp);
		uart_buffer_clean(1);
		
		return;
		
  }else if(uart_recv_count < PKT_MAX_LEN && uart_recv_count > 0)
	{
		uart_buffer_clean(0);
		uart_recv_temp[uart_recv_count] = data;
		uart_recv_count ++;

		return;
  }
}

void usart_msg_handle(void)
{
	if(Data_from_Uart_buffer.pktcnt > 0)
	{ 
		uint8 ops = get_usart_buffer_ops();
    pkt_master_t *req = (pkt_master_t*)&Data_from_Uart_buffer.recvdata[ops][0];
		
		cmd_handle(req);
		
	  Data_from_Uart_buffer.pktcnt -- ;
	}
}
	
/////////////




