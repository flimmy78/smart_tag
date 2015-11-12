
#include "BSP_USART2.h"
#ifdef PRINT_DEBUG

void USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //ʹ��GPIOA��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//ʹ��USART��ʱ��
	/* USART1�Ķ˿����� */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);//����PA2�ɵڶ���������	TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);//����PA3�ɵڶ���������  RX	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2| GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	/* USART1�Ļ������� */
	USART_InitStructure.USART_BaudRate = 115200;              //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);		
	//USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);           //ʹ�ܽ����ж�
	USART_Cmd(USART2, ENABLE);                             //ʹ��USART1
	
	
	//NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	//NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
  //NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStruct);
				
}

//=============================================================================
//�ļ����ƣ�
//���ܸ�Ҫ��USART2�жϺ���
//����˵������
//�������أ���
//=============================================================================
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		
		//usart_data_handle(USART_ReceiveData(USART2));
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	 //
	}
			
}

void USART2_Send(uint8_t *data, uint8_t len)
{
	int i = 0;
	for(i = 0; i<len; i++)
	{
		USART_SendData(USART2, *(data+i));
		 while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
	}
			
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */

#endif
