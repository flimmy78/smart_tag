


#include "a7139.h"
#include "a7139reg.h"



ErrorStatus GIO2_wait(uint8 value)
{
	uint32 timeout = 0;
	ErrorStatus state = ERROR;
  if(value == 0)
  {
		while(A7139_IO_Read_Bit(GIO2) && timeout<20000)
		{
			timeout++;
		}
  }
	if(value == 1)
  {
		while(!A7139_IO_Read_Bit(GIO2) && timeout<20000)
		{
			timeout++;
		}
  }
	if(timeout <20000) state = SUCCESS;
  return state;

}

/*********************************************************************
**get rssi
*********************************************************************/
uint8 rssi_max = 0;
uint8 rssi_min = 0xff;
uint8 A7139_GetRSSI()
{	
	uint8  rssi;
	
	A7139_RX_Exti(DISABLE);
	rssi=(A7139_ReadReg(ADC_REG) & 0x00FF);  //ADC[7:0] is the value of RSSI
	A7139_RX_Exti(ENABLE);
	if(rssi<rssi_min) rssi_min = rssi;
	if(rssi>rssi_max) rssi_max = rssi;
	return rssi;	
}


/*********************************************************************
**into_sleep_mode
*********************************************************************/
void a7139_entry_sleep_mode(void)
{ 
	A7139_RX_Exti(DISABLE);
	StrobeCMD(CMD_STBY);
	Delay_ms(1);	
	StrobeCMD(CMD_SLEEP);
	Delay_ms(10);	
}

/*********************************************************************
** wake_up_from_sleep_mode
*********************************************************************/
void a7139_wake_up_from_sleep_mode(void)
{
	
	StrobeCMD(CMD_STBY);	//wake up
	Delay_ms(5);			//delay 300us for VDD_D stabilized
	StrobeCMD(CMD_RX);
	A7139_RX_Exti(ENABLE);
}

/*********************************************************************
** enter WOR
*********************************************************************/

void WOR_enable_by_sync(void)
{
	A7139_RX_Exti(DISABLE);
	StrobeCMD(CMD_STBY);
  GIO2_wait(0);
	A7139_WritePageA(GIO_PAGEA, (A7139Config_PageA[GIO_PAGEA] & 0xF000) | 0x0045);	//GIO1=FSYNC, GIO2=WTR

	//Real WOR Active Period = (WOR_AC[5:0]+1) x 244us ? X'TAL and Regulator Settling Time
	//Note : Be aware that X?tal settling time requirement includes initial tolerance, 
	//       temperature drift, aging and crystal loading.
	A7139_WritePageA(WOR1_PAGEA, A7139Config_PageA[WOR1_PAGEA]);	//setup WOR Sleep time and Rx time
	
	RCOSC_Cal();     	//RC Oscillator Calibration
	
	A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);	//enable RC OSC & WOR by sync
	
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);				//WORE=1 to enable WOR function
	A7139_RX_Exti(ENABLE);

}

void WOR_enable(void)
{
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
}

void WOR_disable(void)
{
	A7139_RX_Exti(DISABLE);
	
	StrobeCMD(CMD_STBY);
	GIO2_wait(0);
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] & 0xFDFF);				//WORE=1 to disable WOR function
	Delay_ms(5);
	
	A7139_RX_Exti(ENABLE);
}


/*********************************************************************
** set RF WOR睡眠时间
**时间为1s到8s
*********************************************************************/
void A7139_SetSleepTime(uint8 sec)
{
	uint16 value;
	if(sec == 0 || sec >8) return;
	value = 0x80*sec - 1;
	
	//A7139Config_PageA[WOR1_PAGEA] = (A7139Config_PageA[WOR1_PAGEA] & 0xFC00) | value;
}

/*********************************************************************
** set RF channel
**速率为100K，
**ch=0 对应433.921MHZ
**ch=1 对应434.501MHZ
**ch=2 对应433.201MHZ
*********************************************************************/
uint8 A7139_SetChn(uint8 chn)
{
	uint8 vb,vbcf;			//VCO Current
	uint8 vcb, vccf;		//VCO Band
	uint16 tmp;
	uint16 orgVal;
	uint32 freq_data;
	
	if(chn == 0) freq_data = FREQ_433_JOIN;
	else if(chn == 1) freq_data = FREQ_433_BUSS;
	else if(chn == 2) freq_data = FREQ_433_BUGL;
	else return 1;
	
	A7139_RX_Exti(DISABLE);
	
	StrobeCMD(CMD_STBY);
	GIO2_wait(0);
	orgVal = A7139Config[PLL3_REG] & 0x7FFF;
	A7139_WriteReg(PLL3_REG, orgVal);
	
	orgVal = A7139Config[PLL6_REG] & 0x0FFF;
	A7139_WriteReg(PLL6_REG, orgVal);
	
	//433
	orgVal = A7139Config[PLL4_REG] & 0xEFFF;
	A7139_WriteReg(PLL4_REG, orgVal);
	
	orgVal = A7139Config[PLL1_REG] & 0xFF00;
	A7139_WriteReg(PLL1_REG, orgVal | (uint16)(freq_data>>16) );
	A7139_WriteReg(PLL2_REG, (uint16)freq_data);
	
	A7139_WritePageB(IF2_PAGEB, 0x0000);
	
	Delay_ms(5);
	
	
	
	if(vbcf)
	{
		return 1;
	}
	return 0;
}

/*********************************************************************
** a7139 fill fifo
*********************************************************************/
/*********************************************************************
** a7139 fill fifo
*********************************************************************/
#define USE_CRC_MSG
static void A7139_fillFifo(uint8 *buf,uint8 bufSize)
{
#ifdef USE_CRC_MSG
	uint16 crc_value = cal_crc16(buf, bufSize);
#endif
	StrobeCMD(CMD_TFR);	
	A7139_IO_ReSetBit(SCS);
	ByteSend(CMD_FIFO_W);
	while(bufSize--)
	{
		ByteSend(*buf++);
	}
#ifdef USE_CRC_MSG
	ByteSend((uint8)crc_value);
	ByteSend((uint8)(crc_value>>8));
#endif
  A7139_IO_SetBit(SCS);	
}

void A7139_Fill_FIFO(uint8 *buf,uint8 bufSize)
{
	StrobeCMD(CMD_STBY);
  GIO2_wait(0);
	A7139_fillFifo(buf, bufSize); 	//write data to TX FIFO
}

/*********************************************************************
** a7139 sendmsg
*********************************************************************/
ErrorStatus A7139_Send_Msg(uint8 *buf,uint8 bufSize)
{
	StrobeCMD(CMD_TX);
  GIO2_wait(1);
	GIO2_wait(0);
	
	return SUCCESS;
}

/*********************************************************************
** a7139 recv msg
*********************************************************************/
void a7139_recv_handle(void)
{

}


/*********************************************************************
** a7139 init
*********************************************************************/
void InitRF(void)
{
	a7139_GPIO_Init();
	
	//RFEN_Action();
	
  A7139_IO_SetBit(SCS);
	A7139_IO_ReSetBit(SCK);	
	
	StrobeCMD(CMD_RF_RST);	//reset A7139 chip
	Delay_ms(10);
	
  A7139_Config();			//config A7139 chip
	Delay_ms(10);			//for crystal stabilized
	
  A7139_WriteID();		//write ID code
	Delay_ms(1);
	
	StrobeCMD(CMD_STBY);
  A7139_Cal();			//IF and VCO calibration
	
  //StrobeCMD(CMD_RX);
	//Delay10us(1); 
	//A7139_Exti_Init();
	
}










