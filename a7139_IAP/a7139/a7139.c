#include "a7139.h"
#include "stm32f0xx_gpio.h"
#include "main.h"
#include "a7139reg.h"
/*test*/
int current_send = 0;
int current_recv = 0;
//
uint16 RxCnt = 0;
uint8  tmpbuf[64] = {0};

uint32 Err_ByteCnt = 0;
uint32 Err_BitCnt = 0;

uint8  TimeoutFlag = 0;

const uint8  BitCount_Tab[16]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

const uint8  ID_Tab[8]={0x34,0x75,0xC5,0x8C,0xC7,0x33,0x45,0xE7};   //ID code

 uint16  A7139Config[]=		//433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
	0x0021,		//SYSTEM CLOCK register,
	0x0A21,		//PLL1 register,
	0xDA05,		//PLL2 register,	433.301MHz
	0x0000,		//PLL3 register,
	0x0A20,		//PLL4 register,
	0x0024,		//PLL5 register,
	0x0000,		//PLL6 register,
	0x0001,		//CRYSTAL register,
	0x0000,		//PAGEA,
	0x0000,		//PAGEB,
	0x18D4,		//RX1 register, 	IFBW=100KHz, ETH=1	
	0x7009,		//RX2 register, 	by preamble
	0x4400,		//ADC register,	   	
	0x0800,		//PIN CONTROL register,		Use Strobe CMD
	0x4845,		//CALIBRATION register,
	0x20C0		//MODE CONTROL register, 	Use FIFO mode
};

 uint16  A7139Config_PageA[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
	0x1706,		//TX1 register, 	Fdev = 37.5kHz
	0x0000,		//WOR1 register,
	0x0000,		//WOR2 register,
	0x1187,		//RFI register, 	Enable Tx Ramp up/down  
	0x0170,		//PM register,
	0x0302,		//RTH register,
	0x406F,//0x400F,		//AGC1 register,	
	0x0AC0,		//AGC2 register, 
	0x0045,		//GIO register, 	GIO2=WTR, GIO1=FSYNC
	0xD281,		//CKO register
	0x0004,		//VCB register,
	0x0A21,		//CHG1 register, 	430MHz
	0x0022,		//CHG2 register, 	435MHz
	0x003F,		//FIFO register, 	FEP=63+1=64bytes
	0x1507,		//CODE register, 	Preamble=4bytes, ID=4bytes
	0x0000		//WCAL register,
};

 uint16  A7139Config_PageB[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
	0x0B37,		//TX2 register,
	0x8400,		//IF1 register, 	Enable Auto-IF, IF=200KHz
	0x0000,		//IF2 register,
	0x0000,		//ACK register,
	0x0000		//ART register,
};

 uint16  Freq_Cal_Tab[]=
{
	0x0A24,	0xB805,	//470.001MHz
	0x0A26, 0x4805,	//490.001MHz
	0x0A27, 0xD805	//510.001MHz
};


const uint8  PN9_Tab[]=
{   0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0
};	// This table are 64bytes PN9 pseudo random code.


void LedInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int led_key = 0;
void ToggleLED(void)
{
	if(led_key == 1) GPIO_SetBits(GPIOC, GPIO_Pin_13);
   else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	led_key = 1 - led_key;
}


const uint32_t A7139_GPIO_PIN[5] = {A7139_SCS_PIN, A7139_SCK_PIN, A7139_SDIO_PIN, A7139_GDO1_PIN, A7139_GDO2_PIN};

GPIO_TypeDef*  A7139_GPIO_PORT[5] = {A7139_SCS_PORT, A7139_SCK_PORT, A7139_SDIO_PORT, A7139_GDO1_PORT, A7139_GDO2_PORT};

void a7139_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
}

void A7139_RX_Exti(FunctionalState state)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	EXTI_ClearITPendingBit(A7139_GDO2_EXTI_LINE);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStruct);
}
void A7139_Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/*GDO2 for cc1101 recieve*/
	SYSCFG_EXTILineConfig(A7139_GDO2_EXTI_SOURCE, A7139_GDO2_EXTI_PIN);
           
	EXTI_ClearITPendingBit(A7139_GDO2_EXTI_LINE);
	
	EXTI_InitStruct.EXTI_Line = A7139_GDO2_EXTI_LINE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); 
	   	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void SetSDIO_Output(FunctionalState state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(state == ENABLE)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
  }else
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }

}
void SetBit(A7139_IO_TypeDef io)
{
    GPIO_SetBits(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}

void ReSetBit(A7139_IO_TypeDef io)
{
	 GPIO_ResetBits(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}

uint8 Read_Bit(A7139_IO_TypeDef io)
{
  return GPIO_ReadInputDataBit(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}


/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCMD(uint8 cmd)
{
    uint8 i;

    ReSetBit(SCS);
    for(i=0; i<8; i++)
    {
        if(cmd & 0x80)
            SetBit(SDIO);
        else
            ReSetBit(SDIO);

        __ASM volatile ("nop");
        SetBit(SCK);
        __ASM volatile ("nop");
        ReSetBit(SCK);
        cmd<<=1;
    }
	SetBit(SCS);
}

/************************************************************************
**  ByteSend
************************************************************************/
void ByteSend(uint8 src)
{
    uint8 i;

    for(i=0; i<8; i++)
    {
        if(src & 0x80)
            SetBit(SDIO);
        else
            ReSetBit(SDIO);

		    __ASM volatile ("nop");
        SetBit(SCK);
        __ASM volatile ("nop");
        ReSetBit(SCK);
        src<<=1;
    }
}


/************************************************************************
**  ByteRead
************************************************************************/
uint8 ByteRead(void)
{
	uint8 i,tmp;

    //read data code
    SetBit(SDIO);		//SDIO pull high
	 
	  SetSDIO_Output(DISABLE);
    for(i=0; i<8; i++)
    {
        if(Read_Bit(SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        SetBit(SCK);
        __ASM volatile ("nop");
        ReSetBit(SCK);
    }
		SetSDIO_Output(ENABLE);
    return tmp;
}


/************************************************************************
**  A7139_WriteReg
************************************************************************/
void A7139_WriteReg(uint8 address, uint16 dataWord)
{
    uint8 i;

    ReSetBit(SCS);
    address |= CMD_Reg_W;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            SetBit(SDIO);
        else
            ReSetBit(SDIO);

        SetBit(SCK);
        __ASM volatile ("nop"); 
        ReSetBit(SCK);
        address<<=1;
    }
    __ASM volatile ("nop"); 

    //send data word
    for(i=0; i<16; i++)
    {
        if(dataWord & 0x8000)
            SetBit(SDIO);
        else
            ReSetBit(SDIO);

        SetBit(SCK);
        __ASM volatile ("nop"); 
        ReSetBit(SCK);
        dataWord<<=1;
    }
    SetBit(SCS);
}


/************************************************************************
**  A7139_ReadReg
************************************************************************/
uint16 A7139_ReadReg(uint8 address)
{
    uint8 i;
    uint16 tmp;

    ReSetBit(SCS);
    address |= CMD_Reg_R;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            SetBit(SDIO);
        else
            ReSetBit(SDIO);

		__ASM volatile ("nop"); 
        SetBit(SCK);
        __ASM volatile ("nop"); 
        ReSetBit(SCK);

        address<<=1;
    }
    __ASM volatile ("nop"); 
    
    //read data code
    SetBit(SDIO);		//SDIO pull high
		SetSDIO_Output(DISABLE);
    for(i=0; i<16; i++)
    {
        if(Read_Bit(SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

         SetBit(SCK);
        __ASM volatile ("nop"); 
        ReSetBit(SCK);
    }
    SetBit(SCS);
		SetSDIO_Output(ENABLE);
    return tmp;
}

/************************************************************************
**  A7139_WritePageA
************************************************************************/
void A7139_WritePageA(uint8 address, uint16 dataWord)
{
    uint16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEA_REG, dataWord);
}

/************************************************************************
**  A7139_ReadPageA
************************************************************************/
uint16 A7139_ReadPageA(uint8 address)
{
    uint16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7139_ReadReg(PAGEA_REG);
    return tmp;
}

/************************************************************************
**  A7139_WritePageB
************************************************************************/
void A7139_WritePageB(uint8 address, uint16 dataWord)
{
    uint16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  A7139_ReadPageB
************************************************************************/
uint16 A7139_ReadPageB(uint8 address)
{
    uint16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7139_ReadReg(PAGEB_REG);
    return tmp;
}

/*********************************************************************
** Err_State
*********************************************************************/
void Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while(1);
}



/*********************************************************************
** A7139_Config
*********************************************************************/
void A7139_Config(void)
{
	uint8 i;
	uint16 tmp;
  
	A7139_SetDataRate(50);
    for(i=0; i<8; i++)
        A7139_WriteReg(i, A7139Config[i]);

	for(i=10; i<16; i++)
        A7139_WriteReg(i, A7139Config[i]);

    for(i=0; i<16; i++)
        A7139_WritePageA(i, A7139Config_PageA[i]);

	for(i=0; i<5; i++)
        A7139_WritePageB(i, A7139Config_PageB[i]);
  
	
	//for check        
	tmp = A7139_ReadReg(SYSTEMCLOCK_REG);
	if(tmp != A7139Config[SYSTEMCLOCK_REG])
	{
		Err_State();	
	}
	
}

/************************************************************************
**  WriteID
************************************************************************/
void A7139_WriteID(void)
{
	uint8 i;
	uint8 d1, d2, d3, d4;

	ReSetBit(SCS);
	ByteSend(CMD_ID_W);
	for(i=0; i<4; i++)
		ByteSend(ID_Tab[i]);
	SetBit(SCS);

	ReSetBit(SCS);
	ByteSend(CMD_ID_R);
	d1=ByteRead();
	d2=ByteRead();
	d3=ByteRead();
	d4=ByteRead();
  SetBit(SCS);
	
    if((d1!=ID_Tab[0]) || (d2!=ID_Tab[1]) || (d3!=ID_Tab[2]) || (d4!=ID_Tab[3]))
    {
        Err_State();
    }
}

/*********************************************************************
** A7139_Cal
*********************************************************************/
void A7139_Cal(void)
{
    uint8 fb, fcd, fbcf;	//IF Filter
	uint8 vb,vbcf;			//VCO Current
	uint8 vcb, vccf;		//VCO Band
	uint16 tmp;

    //IF calibration procedure @STB state
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0802);			//IF Filter & VCO Current Calibration
    do{
        tmp = A7139_ReadReg(MODE_REG);
    }while(tmp & 0x0802);
	
    //for check(IF Filter)
    tmp = A7139_ReadReg(CALIBRATION_REG);
    fb = tmp & 0x0F;
	fcd = (tmp>>11) & 0x1F;
    fbcf = (tmp>>4) & 0x01;
    if(fbcf)
    {
        Err_State();
    }

	//for check(VCO Current)
    tmp = A7139_ReadPageA(VCB_PAGEA);
	vcb = tmp & 0x0F;
	vccf = (tmp>>4) & 0x01;
	if(vccf)
	{
        Err_State();
    }
    
    
    //RSSI Calibration procedure @STB state
	A7139_WriteReg(ADC_REG, 0x4C00);									//set ADC average=64
    A7139_WritePageA(WOR2_PAGEA, 0xF800);								//set RSSC_D=40us and RS_DLY=80us
	A7139_WritePageA(TX1_PAGEA, A7139Config_PageA[TX1_PAGEA] | 0xE000);	//set RC_DLY=1.5ms
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x1000);			//RSSI Calibration
    do{
        tmp = A7139_ReadReg(MODE_REG);
    }while(tmp & 0x1000);
	A7139_WriteReg(ADC_REG, A7139Config[ADC_REG]);
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA]);
	A7139_WritePageA(TX1_PAGEA, A7139Config_PageA[TX1_PAGEA]);


    //VCO calibration procedure @STB state
	A7139_WriteReg(PLL1_REG, A7139Config[PLL1_REG]);
	A7139_WriteReg(PLL2_REG, A7139Config[PLL2_REG]);
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0004);		//VCO Band Calibration
	do{
		tmp = A7139_ReadReg(MODE_REG);
	}while(tmp & 0x0004);
	
	//for check(VCO Band)
	tmp = A7139_ReadReg(CALIBRATION_REG);
	vb = (tmp >>5) & 0x07;
	vbcf = (tmp >>8) & 0x01;
	if(vbcf)
	{
		Err_State();
	}
}

/*********************************************************************
** A7139_WriteFIFO
*********************************************************************/
void A7139_WriteFIFO(void)
{
	uint8 i;

	StrobeCMD(CMD_TFR);		//TX FIFO address pointer reset

    ReSetBit(SCS);
	ByteSend(CMD_FIFO_W);	//TX FIFO write command
	for(i=0; i <64; i++)
		ByteSend(PN9_Tab[i]);
	SetBit(SCS);
}

/*********************************************************************
** RxPacket
*********************************************************************/
void RxPacket(void)
{
    uint8 i;
    uint8 recv;
    uint8 tmp;
  uint8 right = 0;
	RxCnt++;

	StrobeCMD(CMD_RFR);		//RX FIFO address pointer reset

    ReSetBit(SCS);
	ByteSend(CMD_FIFO_R);	//RX FIFO read command
	for(i=0; i <64; i++)
	{
		tmpbuf[i] = ByteRead();
	}
	SetBit(SCS);

	for(i=0; i<64; i++)
	{
		recv = tmpbuf[i];
		tmp = recv ^ PN9_Tab[i];
		if(tmp!=0)
		{
			Err_ByteCnt++;
			right++;
			Err_BitCnt += (BitCount_Tab[tmp>>4] + BitCount_Tab[tmp & 0x0F]);
		}
   }
	 
	if(right == 0)
	{
		  current_recv++;
			ToggleLED();
  }
}
uint16 adc_value;
void Rx_Read_RSSI(void)
{
	uint16 tmp;
  /*A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0001);
  
	do{
        tmp = A7139_ReadReg(MODE_REG);
   }while(tmp & 0x0001);*/
	 
  adc_value = A7139_ReadReg(RX2_REG);
	adc_value = 150 - (adc_value*48/100) ;

 
}
/*********************************************************************
** initRF
*********************************************************************/
void InitRF(void)
{
    a7139_GPIO_Init();

    StrobeCMD(CMD_RF_RST);	//reset A7139 chip
    A7139_Config();			//config A7139 chip
	  
    Delay_ms(1);			//for crystal stabilized
    A7139_WriteID();		//write ID code
	
	  StrobeCMD(CMD_STBY);
    A7139_Cal();			//IF and VCO calibration
	  
	  StrobeCMD(CMD_RX);
		Delay_ms(10);//Delay10us(1); 
	  A7139_Exti_Init();
	
	  
	
	
	 
}





