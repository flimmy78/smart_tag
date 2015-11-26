
#include "main.h"



Data_from_Uart_to_Rf_t Data_from_Rf_buffer   ={0};

void rf_data_into_buffer(uint8* data)
{
  if( DATA_FROM_USART_BUFFSIZE == Data_from_Rf_buffer.pktcnt) 
    return;
  
  memcpy(&Data_from_Rf_buffer.recvdata[Data_from_Rf_buffer.currentData][0], data, *data + 1 + 2);
  
  Data_from_Rf_buffer.currentData++;
  Data_from_Rf_buffer.pktcnt ++;
  
  if( DATA_FROM_USART_BUFFSIZE == Data_from_Rf_buffer.currentData) Data_from_Rf_buffer.currentData = 0;
}

uint8 rf_get_usart_buffer_ops()
{
  uint8 data_oldest = 0;
  if(Data_from_Rf_buffer.currentData >= Data_from_Rf_buffer.pktcnt) data_oldest = Data_from_Rf_buffer.currentData - Data_from_Rf_buffer.pktcnt;
  else  data_oldest = Data_from_Rf_buffer.currentData + DATA_FROM_USART_BUFFSIZE - Data_from_Rf_buffer.pktcnt;
  return data_oldest;
}
///////////////////////////////////////////////////////////////
Data_from_Uart_to_Rf_t Data_from_Uart_buffer ={0};


void data_into_buffer(uint8* data)
{
  if( DATA_FROM_USART_BUFFSIZE == Data_from_Uart_buffer.pktcnt) 
    return;
  
  memcpy(&Data_from_Uart_buffer.recvdata[Data_from_Uart_buffer.currentData][0], data, *data + 1 + 2);
  
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
		uart_buff_clean_delay = 1000;
	}else if(set == 1){
		//memset(uart_recv_temp, 0, PKT_MAX_LEN);
		uart_recv_count = 0;
	}else{
		uart_buff_clean_delay = 0;
	}
}

void usart_data_handle(uint8 data)
{
	if(0 == uart_recv_count)
  {
		if(data > PKT_MAX_LEN - 1) return;
    uart_recv_temp[0] = data;
		uart_recv_count ++;
		//uart_buffer_clean(0);
		return;
  }else if(uart_recv_count == PKT_MAX_LEN)
	{
		uart_buffer_clean(1);
		return;
  }else if (data == 0xAA && uart_recv_temp[0] == uart_recv_count)
	{
		uart_buffer_clean(2);
		uart_recv_temp[uart_recv_count] = data;
		uart_recv_count ++;
		 USART2_Send(uart_recv_temp, uart_recv_count);
		//uart_recv_temp[0] -=2;

		//data_into_buffer(uart_recv_temp);
		//A7139_Send_Msg(uart_recv_temp, uart_recv_count);
		uart_buffer_clean(1);
		return;
  }else{
		//uart_buffer_clean(0);
		uart_recv_temp[uart_recv_count] = data;
		uart_recv_count ++;
		return;

  }
	
}

static void send_rf_cmd_to_master(pkt_master_t* resp)
{
  uint8 reply_data[12] = {0};
  reply_data[0] = PKT_CONFIG_MAX_LEN + 1 + 2;
  memcpy(&reply_data[1], (uint8*)resp, sizeof(pkt_master_t));
	reply_data[9] = 0xAA;
  USART2_Send(reply_data, PKT_CONFIG_MAX_LEN + 2 +2);
}

void Model_Config(pkt_master_t* req)
{
  uint8 reply_sendbuf[10] = {0};
  uint8 ret = 0;
  
  pkt_master_t *resp = (pkt_master_t*)reply_sendbuf;
  resp->head = C_SYNC_HEAD;
  resp->tail = C_SYNC_TAIL;
  
  if(C_INIT_CMD == req->cmd)
  {
      RfConfig.panId = req->U16_info;
      RfConfig.shortAddr = req->addr;
      RfConfig.channel = req->U8_info;
   
      resp->cmd  = C_CFG_REPLY;
      resp->addr = RfConfig.shortAddr;
      resp ->U8_info =  RfConfig.channel;
      resp ->U16_info = RfConfig.panId;
		
			A7139_SetChn(RfConfig.channel);
		
			send_rf_cmd_to_master(resp);
  }
 
}

void send_alive_cmd_to_master(void)
{
  uint8 sendbuf[10] ={0};
  pkt_master_t *resp = (pkt_master_t*)sendbuf;
  
  resp->head = C_SYNC_HEAD;
  resp->cmd  = 0x55;
  resp->addr = RfConfig.shortAddr;
  resp ->U8_info =  RfConfig.channel;
  resp ->U16_info = RfConfig.panId;
  resp->tail = C_SYNC_TAIL;
  send_rf_cmd_to_master(resp);
}

