
#include "bsp.h"

void SysTick_Init(void)
{
  if(SysTick_Config(SystemCoreClock/1000))
	{
    while(1);
  }
}


//=============================================================================
//文件名称：Delay
//功能概要：延时函数
//参数说明：无
//函数返回：无
//=============================================================================
void SysTime_handle(void)
{ 
	system_flag.sysTimeCount++;
	
	/*systerm delay*/
	if(system_flag.sysTimeDelay>0) system_flag.sysTimeDelay--;
	
	
	//app timer
	if(system_flag.TagTimeCount>0) system_flag.TagTimeCount++;
  if(system_flag.TagTimeTimeout <= system_flag.TagTimeCount) 
	{
		//Tag_state_timer_handle();
	}
	
	//
	len_on_timer_handler();
	
	rf_on_timer_handler();
	
	longdata_wait_timer_handler();
}



void Delay_ms(uint32 nMs)
{
  system_flag.sysTimeDelay = nMs;
	while(system_flag.sysTimeDelay != 0x00);
}


void Delay100us(uint16 n)
{
  uint32_t i;
	uint32_t k = SystemCoreClock/10000;
	
	//n = n*10;
	k=k/5*n;//for take 5 
	
  for(i=0; i<k; i++);
}

/*RCC INIT FUNCTION*/
void Rcc_Init(void)
{
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

void Rcc_DeInit(void)
{
 // RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, DISABLE);
	//RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, DISABLE);
	
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	
	/* Enable the PWR clock */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}



/*LED INIT FUNCTION*/


const uint32_t LED_GPIO_PIN[4] = {LED_RED_PIN,  LED_GREEN_PIN,  LED_YELLOW_PIN,  LED_BLUE_PIN};

GPIO_TypeDef*  LED_GPIO_PORT[4]= {LED_RED_PORT, LED_GREEN_PORT, LED_YELLOW_PORT, LED_BLUE_PORT};

uint8 led_save[4] = {0};


void Set_LedOn(LED_TypeDef led)
{
    GPIO_SetBits(LED_GPIO_PORT[led], LED_GPIO_PIN[led]);
}

void Set_LedOff(LED_TypeDef led)
{
	 GPIO_ResetBits(LED_GPIO_PORT[led], LED_GPIO_PIN[led]);
}

uint8 led_s[4] ={0};
void dis_play_led(LED_TypeDef led)
{
	Set_LedOff(LED_WHITE);
	Set_LedOff(LED_YELLOW);
	Set_LedOff(LED_RED);
	Set_LedOff(LED_GREEN);
  if(led_s[led] == 0 ) Set_LedOn(led);
  led_s[led] = 1 -led_s[led];
}
void turn_all_led_off(void)
{
	Set_LedOff(LED_WHITE);
	Set_LedOff(LED_YELLOW);
	Set_LedOff(LED_RED);
	Set_LedOff(LED_GREEN);
}

void turn_on_led(LED_TypeDef led)
{
  Set_LedOn(led);

}
void Led_toggle(LED_TypeDef led)
{
  if(led_save[led] == 0) Set_LedOn(led);
  else Set_LedOff(led);
	
	led_save[led] = 1 - led_save[led];
}

void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN | LED_YELLOW_PIN | LED_RED_PIN | LED_GREEN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*KEY INIT FUNCTION*/

const uint32_t KEY_GPIO_PIN[4] = {KEY_PUT_PIN, KEY1_PIN, KEY2_PIN,
                                 KEY3_PIN};

GPIO_TypeDef*  KEY_GPIO_PORT[4] = {KEY_PUT_PORT, KEY1_PORT, KEY2_PORT,
                                 KEY3_PORT};

uint8 Key_ReadValue(KEY_TypeDef key)
{
  return GPIO_ReadInputDataBit(KEY_GPIO_PORT[key],KEY_GPIO_PIN[key]);
}
void Button_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = KEY_PUT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}





/*EP12864 INIT FUNCTION*/
const uint32_t EP12864_GPIO_CLK[5] = {EP12864_SID_CLK, EP12864_SCLK_CLK, EP12864_RES_CLK,
                                 EP12864_CS_CLK, EP12864_RS_CLK};

const uint32_t EP12864_GPIO_PIN[5] = {EP12864_SID_PIN, EP12864_SCLK_PIN, EP12864_RES_PIN,
                                 EP12864_CS_PIN, EP12864_RS_PIN};

GPIO_TypeDef* EP12864_GPIO_PORT[5] = {EP12864_SID_PORT, EP12864_SCLK_PORT, EP12864_RES_PORT,
                                 EP12864_CS_PORT, EP12864_RS_PORT};


void EP12865_IO_Init(EP12864_IO_TypeDef io)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;
 
  	/* Configure the GPIO_LED pin */
  	GPIO_InitStructure.GPIO_Pin = EP12864_GPIO_PIN[io];
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  	GPIO_Init(EP12864_GPIO_PORT[io], &GPIO_InitStructure);
}

void EP12864_IO_SetBit(EP12864_IO_TypeDef io)
{
   GPIO_SetBits(EP12864_GPIO_PORT[io], EP12864_GPIO_PIN[io]);
}

void EP12864_IO_RstBit(EP12864_IO_TypeDef io)
{
   GPIO_ResetBits(EP12864_GPIO_PORT[io], EP12864_GPIO_PIN[io]);
}

void EP12864_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	EP12865_IO_Init(SID);
	EP12865_IO_Init(SCLK);
  EP12865_IO_Init(RES);
  EP12865_IO_Init(CS);
	EP12865_IO_Init(RS);

	GPIO_InitStructure.GPIO_Pin = EP12864_BUSY_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//GPIO_PuPd_UP; 
  GPIO_Init(EP12864_BUSY_PORT, &GPIO_InitStructure);
	
}

/*ADC Init*/
#define ADC1_DR_Address                0x40012440
uint16_t RegularConvData_Tab[2];

void Power_ADC_Init(void)
{

	
	DMA_InitTypeDef     DMA_InitStruct;
	
   ADC_InitTypeDef ADC_InitStruct;
   GPIO_InitTypeDef    GPIO_InitStruct;
	
	/* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  /* DMA1 clock enable */

  /* Configure PA.01  as analog input */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PC1,输入时不用设置速率
	
	/* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&RegularConvData_Tab;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize =2;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStruct);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
//   /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE); 
	
	ADC_DeInit(ADC1);
	 ADC_StructInit(&ADC_InitStruct);
	
	 ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	 ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	 ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
   ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	 ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
	 
	 ADC_Init(ADC1, &ADC_InitStruct);
	 //ADC_ChannelConfig(ADC1, ADC_Channel_1  , ADC_SampleTime_55_5Cycles); 
	 ADC_ChannelConfig(ADC1, ADC_Channel_Vbat | ADC_Channel_Vrefint,ADC_SampleTime_71_5Cycles); 
   ADC_VrefintCmd(ENABLE);
	 ADC_VbatCmd(ENABLE);
	/* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
	ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
	
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
	
	
	Delay100us(10);

	ADC_StopOfConversion(ADC1);
	ADC_Cmd(ADC1, DISABLE); 
	ADC_DMACmd(ADC1, DISABLE);
	DMA_Cmd(DMA1_Channel1, DISABLE);
	ADC_VrefintCmd(DISABLE);
	ADC_VbatCmd(DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , DISABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PC1,输入时不用设置速率
	
	system_flag.sys_power = (245*RegularConvData_Tab[0]/RegularConvData_Tab[1]);
	
}

/*EXTI Init*/
void nvic_rf_rx(FunctionalState state)
{
   NVIC_InitTypeDef NVIC_InitStruct;
   NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
   NVIC_InitStruct.NVIC_IRQChannelCmd = state;
   NVIC_Init(&NVIC_InitStruct);
}
void GPIO_Exti_Init(void)
{
     GPIO_InitTypeDef GPIO_InitStruct; 
     EXTI_InitTypeDef EXTI_InitStruct;
     NVIC_InitTypeDef NVIC_InitStruct;

	   /*KEY PUT*/
	   SYSCFG_EXTILineConfig(KEY_PUT_EXTI_SOURCE, KEY_PUT_EXTI_PIN);
	   EXTI_ClearITPendingBit(KEY_PUT_EXTI_LINE);
	
     EXTI_InitStruct.EXTI_Line = KEY_PUT_EXTI_LINE;
     EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
     EXTI_InitStruct.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStruct); 
		 
		 SYSCFG_EXTILineConfig(KEY1_EXTI_SOURCE, KEY1_EXTI_PIN);
	   EXTI_ClearITPendingBit(KEY1_EXTI_LINE);
	
     EXTI_InitStruct.EXTI_Line = KEY1_EXTI_LINE;
     EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
     EXTI_InitStruct.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStruct); 
		 
		 SYSCFG_EXTILineConfig(KEY2_EXTI_SOURCE, KEY2_EXTI_PIN);
	   EXTI_ClearITPendingBit(KEY2_EXTI_LINE);
	
     EXTI_InitStruct.EXTI_Line = KEY2_EXTI_LINE;
     EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
     EXTI_InitStruct.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStruct); 
		 
		  SYSCFG_EXTILineConfig(KEY3_EXTI_SOURCE, KEY3_EXTI_PIN);
	   EXTI_ClearITPendingBit(KEY3_EXTI_LINE);
	
     EXTI_InitStruct.EXTI_Line = KEY3_EXTI_LINE;
     EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
     EXTI_InitStruct.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStruct); 
		 
		 
		 NVIC_InitStruct.NVIC_IRQChannel = EXTI2_3_IRQn;
     NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
     NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStruct);
		 
		 NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
     NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
     NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStruct);
		 
	
		 
}



void IAP_Set(void)
{
   uint32_t i = 0;
      
  for(i = 0; i < 48; i++)
  {
    *((uint32_t*)(0x20000000 + (i << 2)))=*(__IO uint32_t*)(STORE_APP_RUN_ADDR + (i<<2));
	}
  /* Enable the SYSCFG peripheral clock*/ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
	
	/* Remap SRAM at 0x00000000 */
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
}	


void system_reset(void)
{
	NVIC_SystemReset();

}

void Board_Init(void)
{
	IAP_Set();
	
	SysTick_Init();	

	Rcc_Init();
	
  LED_Init();
  Button_Init();
	
	EP12864_Init();
	Init_EPaper(1);
  lcd_sleep_mode();
	
	RTC_LSE_Config();
	
	InitRF();
	
	GPIO_Exti_Init();
	
 
}


