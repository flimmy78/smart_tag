#include "a7139.h"
#include "stm32f0xx_gpio.h"
#include "app_config.h"
#include "a7139reg.h"
#include "app_typedef.h"
#include "bsp.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"

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
	0x8000,		//ADC register,	   	
	0x0800,		//PIN CONTROL register,		Use Strobe CMD
	0xCC45,		//CALIBRATION register,
	0x20C0		//MODE CONTROL register, 	Use FIFO mode
};

uint16  A7139Config_PageA[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
	0x1f06,	//0x1706,		//TX1 register, 	Fdev = 37.5kHz
	0xFFFF,		//WOR1 register,
	0xF800,		//WOR2 register,
	0x1187,		//RFI register, 	Enable Tx Ramp up/down  
	0x0170,		//PM register,
	0x0201,		//RTH register,
	0x400F,//0x400F,		//AGC1 register,	
	0x2AC0,		//AGC2 register, 
	0x0045,		//GIO register, 	GIO2=WTR, GIO1=FSYNC
	0xD381,		//CKO register
	0x0004,		//VCB register,
	0x0A21,		//CHG1 register, 	430MHz
	0x0022,		//CHG2 register, 	435MHz
	0x003F,		//FIFO register, 	FEP=63+1=64bytes
	0x150F,		//CODE register, 	Preamble=4bytes, ID=4bytes
	0x0000		//WCAL register,
};

uint16  A7139Config_PageB[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
	0x0B37,//0x0B37,		//TX2 register,
	0x8400,		//IF1 register, 	Enable Auto-IF, IF=200KHz
	0x0000,		//IF2 register,
	0x0000,		//ACK register,
	0x0000		//ART register,
};


const uint32_t A7139_GPIO_PIN[6] =  {A7139_SCS_PIN,  A7139_SCK_PIN,  A7139_SDIO_PIN,  A7139_GDO1_PIN,  A7139_GDO2_PIN,  A7139_RFEN_PIN};
GPIO_TypeDef*  A7139_GPIO_PORT[6] = {A7139_SCS_PORT, A7139_SCK_PORT, A7139_SDIO_PORT, A7139_GDO1_PORT, A7139_GDO2_PORT, A7139_RFEN_PORT};

void a7139_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = A7139_SCS_PIN | A7139_SCK_PIN | A7139_SDIO_PIN | A7139_RFEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = A7139_GDO1_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
  GPIO_Init(A7139_GDO1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = A7139_GDO2_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
  GPIO_Init(A7139_GDO2_PORT, &GPIO_InitStructure);

}
void SetSDIO_Output(FunctionalState state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(state == ENABLE)
	{
		GPIO_InitStructure.GPIO_Pin = A7139_SDIO_PIN ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_Init(A7139_SDIO_PORT, &GPIO_InitStructure);
  }else
	{
		GPIO_InitStructure.GPIO_Pin = A7139_SDIO_PIN ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(A7139_SDIO_PORT, &GPIO_InitStructure);
  }

}
void A7139_IO_SetBit(A7139_IO_TypeDef io)
{
    GPIO_SetBits(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}

void A7139_IO_ReSetBit(A7139_IO_TypeDef io)
{
	 GPIO_ResetBits(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}

uint8 A7139_IO_Read_Bit(A7139_IO_TypeDef io)
{
  return GPIO_ReadInputDataBit(A7139_GPIO_PORT[io], A7139_GPIO_PIN[io]);
}


void A7139_RX_Exti(FunctionalState state)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	EXTI_ClearITPendingBit(A7139_GDO1_EXTI_LINE);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStruct);
}

void A7139_Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	
	/*GDO1 for a7139 ID CODE MATCH recieve*/
	SYSCFG_EXTILineConfig(A7139_GDO1_EXTI_SOURCE, A7139_GDO1_EXTI_PIN);
           
	EXTI_ClearITPendingBit(A7139_GDO1_EXTI_LINE);
	
	EXTI_InitStruct.EXTI_Line = A7139_GDO1_EXTI_LINE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; 
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); 
	   	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*********************************************************************
** 放电电路放电，在上电时执行
*********************************************************************/

void RFEN_Action(void)
{
	A7139_IO_SetBit(RFEN);
	Delay100us(100);
	A7139_IO_ReSetBit(RFEN);


}

/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCMD(uint8 cmd)
{
    uint8 i;

    A7139_IO_ReSetBit(SCS);
    for(i=0; i<8; i++)
    {
        if(cmd & 0x80)
            A7139_IO_SetBit(SDIO);
        else
            A7139_IO_ReSetBit(SDIO);

        __ASM volatile ("nop");
        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop");
        A7139_IO_ReSetBit(SCK);
        cmd<<=1;
    }
	A7139_IO_SetBit(SCS);
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
            A7139_IO_SetBit(SDIO);
        else
            A7139_IO_ReSetBit(SDIO);

		    __ASM volatile ("nop");
        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop");
        A7139_IO_ReSetBit(SCK);
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
    A7139_IO_SetBit(SDIO);		//SDIO pull high
	 
	  SetSDIO_Output(DISABLE);
    for(i=0; i<8; i++)
    {
        if(A7139_IO_Read_Bit(SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop");
        A7139_IO_ReSetBit(SCK);
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

    A7139_IO_ReSetBit(SCS);
    address |= CMD_Reg_W;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            A7139_IO_SetBit(SDIO);
        else
            A7139_IO_ReSetBit(SDIO);

        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop"); 
        A7139_IO_ReSetBit(SCK);
        address<<=1;
    }
    __ASM volatile ("nop"); 

    //send data word
    for(i=0; i<16; i++)
    {
        if(dataWord & 0x8000)
            A7139_IO_SetBit(SDIO);
        else
            A7139_IO_ReSetBit(SDIO);

        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop"); 
        A7139_IO_ReSetBit(SCK);
        dataWord<<=1;
    }
    A7139_IO_SetBit(SCS);
}


/************************************************************************
**  A7139_ReadReg
************************************************************************/
uint16 A7139_ReadReg(uint8 address)
{
    uint8 i;
    uint16 tmp;

    A7139_IO_ReSetBit(SCS);
    address |= CMD_Reg_R;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            A7139_IO_SetBit(SDIO);
        else
            A7139_IO_ReSetBit(SDIO);

		__ASM volatile ("nop"); 
        A7139_IO_SetBit(SCK);
        __ASM volatile ("nop"); 
        A7139_IO_ReSetBit(SCK);

        address<<=1;
    }
    __ASM volatile ("nop"); 
    
    //read data code
    A7139_IO_SetBit(SDIO);		//SDIO pull high
		SetSDIO_Output(DISABLE);
    for(i=0; i<16; i++)
    {
        if(A7139_IO_Read_Bit(SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

         A7139_IO_SetBit(SCK);
        __ASM volatile ("nop"); 
        A7139_IO_ReSetBit(SCK);
    }
    A7139_IO_SetBit(SCS);
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
	uint8  ID_Tab[8]={0x34,0x75,0xC5,0x8C,0xC7,0x33,0x45,0xE7};   //ID code

	uint8 i;
	uint8 d1, d2, d3, d4;

	A7139_IO_ReSetBit(SCS);
	ByteSend(CMD_ID_W);
	for(i=0; i<4; i++)
		ByteSend(ID_Tab[i]);
	A7139_IO_SetBit(SCS);

	A7139_IO_ReSetBit(SCS);
	ByteSend(CMD_ID_R);
	d1=ByteRead();
	d2=ByteRead();
	d3=ByteRead();
	d4=ByteRead();
  A7139_IO_SetBit(SCS);
	
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
	
	if(fbcf) Err_State();
 
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
** A7139 RCOSC Cal
*********************************************************************/

void RCOSC_Cal(void)
{
	uint16 tmp;

	A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);		//enable RC OSC

	while(1)
	{
		A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA] | 0x0001);	//set ENCAL=1 to start RC OSC CAL
		do{
			tmp = A7139_ReadPageA(WCAL_PAGEA);
		}while(tmp & 0x0001);
			
		tmp = (A7139_ReadPageA(WCAL_PAGEA) & 0x03FF);		//read NUMLH[8:0]
		tmp >>= 1;	
		
		if((tmp > 183) && (tmp < 205))		//NUMLH[8:0]=194+-10 (PF8M=6.4M)
		//if((tmp > 232) && (tmp < 254))	//NUMLH[8:0]=243+-10 (PF8M=8M)
		{
			break;
		}
	}
}






