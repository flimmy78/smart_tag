

#include "a7139.h"
#include "a7139_api.h"
#include "a7139reg.h"

static uint8 A7139_RCOSC_Cal(void)
{
	  uint8  retry = 0xFF;
	  uint16 calbrtVal,t_retry=0xFFFF;
	  		//RCOSC_E[4:4] = 1,enable internal RC Oscillator
	  A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);
	  do{
		  		//ENCAL[0:0] = 1,then start RC OSC Calbrt
		  A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA] | 0x0001);
		  do{
		  	  calbrtVal = A7139_ReadPageA(WCAL_PAGEA) & 0x0001;
		  }while(calbrtVal && t_retry--);
		  		//read NUMLH[9:1]
		  calbrtVal = (A7139_ReadPageA(WCAL_PAGEA) & 0x03FF) >> 1;
		  if(calbrtVal>186 && calbrtVal<198)
			   return OK_RCOSC_CAL;
	  }while(retry--);
	  return ERR_RCOSC_CAL;
}

uint8 A7139_SetCID(uint32 id)
{
	ReSetBit(SCS);
	ByteSend(CMD_ID_W);
	ByteSend((uint8)(id>>24));
	ByteSend((uint8)(id>>16));
	ByteSend((uint8)(id>>8));
	ByteSend((uint8)id);
	SetBit(SCS);
	return 0;
}

uint16 A7139_ReadPID(void)
{
	uint16 pid;
	uint16 pagAddr = TX2_PAGEB << 7;
	pagAddr|=A7139Config[CRYSTAL_REG] & 0xF7CF;
	A7139_WriteReg(CRYSTAL_REG, pagAddr);
	pid = A7139_ReadReg(PAGEB_REG);
	return pid;
}

uint8 A7139_SetDataRate(uint8 datRate)
{
	
	switch(datRate)
	{
		
		case 2:
		{
			 		//CSC[2:0]=[011],Fcsck=3.2MHz
					//IFBW[3:2]=[00],50Kps 
					//SDR[15:9]=0x00,DCK=50kHz,Fdev = 18.75kHz

			A7139Config[SYSTEMCLOCK_REG] = 0x6221;
			A7139Config_PageA[TX1_PAGEA] = 0x1505;
			A7139Config_PageA[PM_PAGEA] = 0x8170;
		}
		break;
		
		case 10:
		{
			 		//CSC[2:0]=[001] ,Fcsck=6.4MHz
					//IFBW[3:2]=[01],100kHz
					//SDR[15:9]=0x00,DCK=100Kps

			A7139Config[SYSTEMCLOCK_REG] = 0x1221;
			A7139Config[RX1_REG] = 0x18D4;
			A7139Config_PageA[TX1_PAGEA] = 0x1706;
		}
		break;
		case 50:
		{
			 		//CSC[2:0]=[011],Fcsck=3.2MHz
					//IFBW[3:2]=[00],50Kps 
					//SDR[15:9]=0x00,DCK=50kHz,Fdev = 18.75kHz

			A7139Config[SYSTEMCLOCK_REG] = 0x0023;
			A7139Config[RX1_REG] = 0x18D0;
			A7139Config_PageA[TX1_PAGEA] = 0x1606;
			A7139Config_PageA[PM_PAGEA] = 0x8170;
			A7139Config_PageB[IF1_PAGEB] = 0x8200;
		}
		break;
		
		case 100:												
		{
					//CSC[2:0]=[001] ,Fcsck=6.4MHz
					//IFBW[3:2]=[01],100kHz
					//SDR[15:9]=0x00,DCK=100Kps
			A7139Config[SYSTEMCLOCK_REG] = 0x0021;
			A7139Config[RX1_REG] = 0x18D4;
			A7139Config_PageA[TX1_PAGEA] = 0x1706;
			
		}
		break;
		
		default:
			return 0x01;//ERR_PARAM;
	}
	
	return 0;
}

uint8 A7139_SetPackLen(uint8 len)
{
	uint16 pagVal;
	StrobeCMD(CMD_STBY);
			//FEP[7:0]
	pagVal = A7139Config_PageA[FIFO_PAGEA] & 0xFF00;
	A7139_WritePageA(FIFO_PAGEA,pagVal|(len-1));
			//FEP[13:8]
	pagVal = A7139Config_PageA[VCB_PAGEA] & 0xC0FF;
	A7139_WritePageA(VCB_PAGEA,pagVal);
	return 0;			
}

uint8 A7139_SetCIDLen(uint8 len)
{
	switch(len)
	{
		case 2:
			{
				A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] & 0xBFFB);
			}
			break;
		case 4:
			{
				A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] & 0xBFFB);
				A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] | 0x0004);
			}
			break;
		default :
		return 0x01;//ERR_PARAM;
	}
	return 0;
}

void A7139_fillFifo(uint8 *buf,uint8 bufSize)
{
	StrobeCMD(CMD_TFR);	
  Delay_ms(1);
	
	ReSetBit(SCS);
	ByteSend(CMD_FIFO_W);
	while(bufSize--) ByteSend(*buf++);
  SetBit(SCS);	
}

void A7139_ReadFIFO1(uint8 *buf,uint8 bufSize)
{
	StrobeCMD(CMD_RFR);
	Delay_ms(1);
	ReSetBit(SCS);
	ByteSend(CMD_FIFO_R);
	while(bufSize--)
		*buf++ = ByteRead(); 
	SetBit(SCS);								 
}

uint8 A7139_IsBatteryLow(uint8 low2_x)
{
	uint16 pagVal;
	if(low2_x<0x02 || low2_x>0x07)
		return 0x01;//ERR_PARAM;
	StrobeCMD(CMD_STBY);
			//BVT[3:1] BDS[0:0]
	pagVal= A7139Config[PM_PAGEA] & 0xFFF0;
	A7139_WritePageA(PM_PAGEA,pagVal | (low2_x << 1) | 0x01);
	Delay_ms(1);//Delay_us(10); //delay 5us at least 
			//read VBD[7:7]
	return !((A7139_ReadPageA(WOR1_PAGEA) & 0x0080) >> 7);
}
uint8 A7139_GetRSSI()
{	
	uint8  rssi;
	uint16 t_retry = 0xFFFF;
		//entry RX mode
	StrobeCMD(CMD_RX);	
			//CDM[8:8] = 0
	A7139_WriteReg(ADC_REG,A7139Config[ADC_REG] & 0xFEFF);
			//ADCM[0:0] = 1
	A7139_WriteReg(MODE_REG,A7139_ReadReg(MODE_REG) | 0x0001);
	do
	{
		rssi = A7139_ReadReg(MODE_REG) & 0x0001; //ADCM auto clear when measurement done
			
	}while(t_retry-- && rssi);
	if(t_retry>0)
		rssi=(A7139_ReadReg(ADC_REG) & 0x00FF);  //ADC[7:0] is the value of RSSI
	else
		rssi = ERR_GET_RSSI;
	StrobeCMD(CMD_STBY);
	return rssi;		
}

uint8 A7139_WOT(void)
{
	if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
		return 0x04;//ERR_RCOSC_CAL;
	StrobeCMD(CMD_STBY);
		//GIO1=FSYNC, GIO2=WTR	
	A7139_WritePageA(GIO_PAGEA, 0x0045);
		//setup WOT Sleep time
	A7139_WritePageA(WOR1_PAGEA, 0x027f);
		//WMODE=1 select WOT function
	A7139_WriteReg(PIN_REG, A7139Config[PIN_REG] | 0x0400);
		//WORE=1 to enable WOT function		
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
	//while(1); //注意此处为死循环，代码只为演示之用，用户必须按业务实际逻辑进行需改
	return 0;
}
uint8 A7139_WOR_BySync(void)
{
	StrobeCMD(CMD_STBY);
			//GIO1=FSYNC, GIO2=WTR	
	A7139_WritePageA(GIO_PAGEA, 0x0045);
			//setup WOR Sleep time and Rx time
	A7139_WritePageA(WOR1_PAGEA, 0xFC05);
	if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
		return ERR_RCOSC_CAL;
			//enable RC OSC & WOR by sync
	A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);
			//WORE=1 to enable WOR function
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
	
	//while(GIO1==0);		//Stay in WOR mode until receiving preamble(preamble ok)
	return 0;
}

uint8 A7139_WOR_ByPreamble(void)
{
	StrobeCMD(CMD_STBY);
	A7139_WritePageA(GIO_PAGEA, 0x004D);	//GIO1=PMDO, GIO2=WTR

	//Real WOR Active Period = (WOR_AC[5:0]+1) x 244us,XTAL and Regulator Settling Time
	//Note : Be aware that Xtal settling time requirement includes initial tolerance, 
	//       temperature drift, aging and crystal loading.
	A7139_WritePageA(WOR1_PAGEA, 0xFC05);	//setup WOR Sleep time and Rx time
			 	//RC Oscillator Calibration
	if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
		return ERR_RCOSC_CAL;
	A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0030);	//enable RC OSC & WOR by preamble
	
	A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);				//WORE=1 to enable WOR function
	
	//while(GIO1==0);		//Stay in WOR mode until receiving preamble(preamble ok)
	return 0;
}


ErrorStatus A7139_Send_Msg(uint8 *buf,uint8 bufSize)
{
	int timeout = 0;
	
	StrobeCMD(CMD_STBY);
	A7139_RX_Exti(DISABLE);
	A7139_fillFifo(buf, bufSize); 	//write data to TX FIFO
  
	StrobeCMD(CMD_TX);
  
	Delay_ms(1);//Delay10us(1);
		
	while(Read_Bit(GIO2) && timeout < 2000) 		//wait transmit completed
  {
    timeout++;
		Delay_ms(1);
  }
	if(timeout >= 2000) return ERROR;
	
	StrobeCMD(CMD_RX);
	A7139_RX_Exti(ENABLE);
	
  Delay_ms(1);//Delay10us(1);
	
	return SUCCESS;
}

void frame_pkt_handle(uint8 *pkt_tmp, uint8 pkt_len)
{
  // if(*(pkt_tmp+pkt_len) == 0xaa) 
	USART2_Send(pkt_tmp, pkt_len+1);
}

void a7139_recv_handle(void)
{
	uint8 pkt_len = 0, i;
	uint8 pkt_tmp[64] = {0};
	
	StrobeCMD(CMD_RFR);		//RX FIFO address pointer reset

  ReSetBit(SCS);
	ByteSend(CMD_FIFO_R);	//RX FIFO read command
	
/*
  for(i=0; i< 64 ; i++)
	{
		pkt_tmp[i] = ByteRead();
	}
	
	frame_pkt_handle(pkt_tmp, 64);
	return;
*/
	pkt_len = ByteRead();
	if(pkt_len == 0 || pkt_len > FRAME_PKT_LEN - 1) return;
	
	pkt_tmp[0] = pkt_len;
	for(i=1; i< pkt_len + 1; i++)
	{
		pkt_tmp[i] = ByteRead();
	}
	
	SetBit(SCS);
  
	frame_pkt_handle(pkt_tmp, pkt_len);
}

void rf_handle(void)
{
   
}









