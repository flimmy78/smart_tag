

#include "bsp.h"

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
	StrobeCMD(CMD_RX);
	GIO2_wait(1);
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
	Delay100us(10);	
	StrobeCMD(CMD_SLEEP);
	Delay100us(100);	
}

/*********************************************************************
** wake_up_from_sleep_mode
*********************************************************************/
void a7139_wake_up_from_sleep_mode(void)
{
	StrobeCMD(CMD_STBY);	//wake up
	Delay100us(50);			//delay 300us for VDD_D stabilized
	StrobeCMD(CMD_RX);
	GIO2_wait(1);
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
	Delay100us(50);
	
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
	
	A7139Config_PageA[WOR1_PAGEA] = (A7139Config_PageA[WOR1_PAGEA] & 0xFC00) | value;
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
	uint16 tmp, rssi_mask;
	uint16 orgVal;
	uint32 freq_data;
	
	if(chn == 0) freq_data = FREQ_433_JOIN;
	else if(chn == 1) freq_data = FREQ_433_BUSS;
	else if(chn == 2) freq_data = FREQ_433_BUGL;
	else return 1;
	
	A7139_RX_Exti(DISABLE);
	StrobeCMD(CMD_STBY);
	GIO2_wait(0);
	//VCO calibration procedure @STB state
	A7139_WriteReg(PLL1_REG, (A7139Config[PLL1_REG] & 0xFF00) | (uint16)(freq_data>>16));
	A7139_WriteReg(PLL2_REG, (uint16)freq_data);
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0004);		//VCO Band Calibration
	do{
		tmp = A7139_ReadReg(MODE_REG);
	}while(tmp & 0x0004);
	
	//for check(VCO Band)
	tmp = A7139_ReadReg(CALIBRATION_REG);
	vb = (tmp >>5) & 0x07;
	vbcf = (tmp >>8) & 0x01;
	
	rssi_mask=A7139_ReadPageA(RFI_PAGEA);
	
	
	system_flag.rssi_high_mask = (uint8)(rssi_mask>>8);
	system_flag.rssi_low_mask = (uint8)(rssi_mask);
	
	StrobeCMD(CMD_RX);
	GIO2_wait(1);
	A7139_RX_Exti(ENABLE);
	if(vbcf)
	{
		return 1;
	}
	return 0;
}

/*********************************************************************
** a7139 fill fifo
*********************************************************************/
static void A7139_fillFifo(uint8 *buf,uint8 bufSize)
{
#ifdef USE_CRC_MSG
	uint16 crc_value;
	crc_value = cal_crc16(buf, bufSize);
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


/*********************************************************************
** a7139 sendmsg
*********************************************************************/
ErrorStatus cca_detect(void)
{
  uint8 rssi_value;
	uint8 rssi_rts = 12;
	ErrorStatus state = ERROR;
	
	do{
		rssi_value = A7139_GetRSSI();
		if(rssi_value < system_flag.rssi_high_mask + 10) 
		{
			rssi_rts--;
		}
		else 
		{
			return ERROR;
		}
		Delay100us(10);
	}while(rssi_rts>0);
	
	return SUCCESS;
}
uint8 times = 0;
	uint16 csma_delay = 0;
	uint16 delay_max = 1;
ErrorStatus csma_detect(void)
{
	times = 0;
	 csma_delay = 0;
	 delay_max = 1;
	for(times = 0; times<10; times++)
	{
		delay_max = delay_max*2;
		if(SUCCESS == cca_detect()) return SUCCESS;
		csma_delay = get_random(0, delay_max);
		Delay100us(10*csma_delay);
	}
	return ERROR;


}

ErrorStatus A7139_Send_Msg(uint8 *buf,uint8 bufSize)
{
	uint8 rssi_value;
	uint8 rssi_rts = 6;
	uint8 rssi_retry = 0;
  ErrorStatus state = ERROR;

	if(SUCCESS != csma_detect()) return ERROR;
	
	A7139_RX_Exti(DISABLE);

	StrobeCMD(CMD_STBY);
	state = GIO2_wait(0);
	
	A7139_fillFifo(buf, bufSize); 	//write data to TX FIFO
	
	StrobeCMD(CMD_TX);
	state = GIO2_wait(1);
	state =GIO2_wait(0);

	StrobeCMD(CMD_RX);
	state = GIO2_wait(1);
  
	A7139_RX_Exti(ENABLE);

	return state;
}

/*********************************************************************
** a7139 recv msg
*********************************************************************/

void a7139_recv_handle(void)
{
	uint8 pkt_len = 0, i;
	uint8 pkt_tmp[64] = {0};
  uint8 has_msg = 0;
  uint8 rssi;

#ifdef USE_CRC_MSG
  uint16 crc_value;
  uint16 crc_value_cal;
#endif

  rssi =(A7139_ReadReg(ADC_REG) & 0x00FF);
	StrobeCMD(CMD_RFR);		//RX FIFO address pointer reset

  A7139_IO_ReSetBit(SCS);
	ByteSend(CMD_FIFO_R);	//RX FIFO read command

	pkt_len = ByteRead();
 
  if(pkt_len > 0 && pkt_len <= FRAME_PKT_LEN - 1)
	{
			pkt_tmp[0] = pkt_len;
			for(i=0; i< pkt_len + 2/*crc*/; i++)
			{
				pkt_tmp[i+1] = ByteRead();
			}
			
			has_msg = 1;
			
#ifdef USE_CRC_MSG
			crc_value = (uint16)(pkt_tmp[i]<<8) + (uint16)pkt_tmp[i - 1];
			crc_value_cal = cal_crc16(pkt_tmp, pkt_len + 1);
			if(crc_value_cal != crc_value) has_msg = 0;
#endif
	}
	
	A7139_IO_SetBit(SCS);
	
	/*for test*/if(has_msg == 1 && pkt_len == 11 && pkt_tmp[1] == 0xe3 && pkt_tmp[2] == 0xe4)
	{
		cale_send_and_recv(pkt_tmp, pkt_len, rssi);
	}
	if(has_msg == 1) frame_pkt_handle(pkt_tmp, pkt_len, 0);
	
	if(system_flag.sys_sleep == 1) return;
  StrobeCMD(CMD_RX);
	GIO2_wait(1);
}
void Enter_RX(void)
{ 
	ErrorStatus state = ERROR;
	StrobeCMD(CMD_RX);
  state = GIO2_wait(1);

}


/*********************************************************************
** a7139 init
*********************************************************************/
void InitRF(void)
{
	uint16 rssi_mask = 0;
	a7139_GPIO_Init();
	
  A7139_IO_SetBit(SCS);
	A7139_IO_ReSetBit(SCK);	
	
	StrobeCMD(CMD_RF_RST);	//reset A7139 chip
	Delay100us(10);
	
  A7139_Config();			//config A7139 chip
	Delay100us(10);			//for crystal stabilized
	
  A7139_WriteID();		//write ID code
	Delay100us(10);
	
	StrobeCMD(CMD_STBY);
	Delay100us(10);
  A7139_Cal();			//IF and VCO calibration
	
	/*Get RSSI MASK*/
  rssi_mask=A7139_ReadPageA(RFI_PAGEA);
	
	system_flag.rssi_high_mask = (uint8)(rssi_mask>>8);
	system_flag.rssi_low_mask = (uint8)(rssi_mask);
	StrobeCMD(CMD_RX);
	GIO2_wait(1);
  A7139_Exti_Init();
	
	
}










