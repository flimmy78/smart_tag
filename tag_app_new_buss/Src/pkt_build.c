

#include "bsp.h"


static uint8 RfBuildHeader(pMpdu_TxBuff_t pTxMpdu)
{
	uint8 src_addr_mode = (pTxMpdu->fcf&FCF_SRC_ADDR_MODE_MASK)>>FCF_SRC_ADDR_MODE_OFFSET;
	uint8 dst_addr_mode = (pTxMpdu->fcf&FCF_DST_ADDR_MODE_MASK)>>FCF_DST_ADDR_MODE_OFFSET;

	pTxMpdu->MpduPayload[1] = LO_UINT16(pTxMpdu->fcf);
	pTxMpdu->MpduPayload[2] = HI_UINT16(pTxMpdu->fcf);
	pTxMpdu->MpduPayload[3] = pTxMpdu->txSeqNumber;
	pTxMpdu->MpduPayload[4] = LO_UINT16(pTxMpdu->DestPanId);
	pTxMpdu->MpduPayload[5] = HI_UINT16(pTxMpdu->DestPanId);
	pTxMpdu->MpduPayload[6] = LO_UINT16(pTxMpdu->DestshortAddr);
	pTxMpdu->MpduPayload[7] = HI_UINT16(pTxMpdu->DestshortAddr);
    if(dst_addr_mode==SHORT_ADDR)
    {
        if(src_addr_mode==LONG_ADDR)
        {
        	pTxMpdu->MpduPayload[0] = pTxMpdu->ApduLen + BASIC_RF_PACKET_LONGHEAD_SIZE;
        	pTxMpdu->MpduPayload[8] = tag_mac_Addr[0];
        	pTxMpdu->MpduPayload[9] = tag_mac_Addr[1];
        	pTxMpdu->MpduPayload[10] = tag_mac_Addr[2];
        	pTxMpdu->MpduPayload[11] = tag_mac_Addr[3];
        	pTxMpdu->MpduPayload[12] = tag_mac_Addr[4];
        	pTxMpdu->MpduPayload[13] = tag_mac_Addr[5];
        	pTxMpdu->MpduPayload[14] = tag_mac_Addr[6];
        	pTxMpdu->MpduPayload[15] = tag_mac_Addr[7];
            return 16;
        }
        else
        {
        	pTxMpdu->MpduPayload[0] = pTxMpdu->ApduLen + BASIC_RF_PACKET_SHORTHEAD_SIZE;
        	pTxMpdu->MpduPayload[8] = LO_UINT16(tag_flash_info.short_addr);
        	pTxMpdu->MpduPayload[9] = HI_UINT16(tag_flash_info.short_addr);
            return 10;
        }
    }
    else
    {
    	return 0;
    }
}

static uint8 RfBuildMpdu(pMpdu_TxBuff_t pTxMpdu)
{
    uint8 hdrLength, n;

    hdrLength = RfBuildHeader(pTxMpdu);

    for(n=0;n<pTxMpdu->ApduLen;n++)
    {
    	pTxMpdu->MpduPayload[hdrLength+n] = pTxMpdu->ApduPayload[n];
    }
    return hdrLength + pTxMpdu->ApduLen; // total mpdu length
}

static ErrorStatus RfTransmitPacket(void)
{
	ErrorStatus state;
    uint8 status = 0;
	  uint8 length = 0;
	
	 length = RfBuildMpdu(&TxMpdu);
	
   if(length>64) return ERROR;

  state =  A7139_Send_Msg(TxMpdu.MpduPayload, length);
	
	if(SUCCESS == state)
	{
		sys_info_print("send MSG", 8);
		
	}
	return state;
}

ErrorStatus RfSendPacket(void)
{
	uint8 send_timeout=0;
  ErrorStatus status = ERROR;
	while(send_timeout < TIMEOUT_RETRANSMIT)
	{
    if(SUCCESS == RfTransmitPacket()){
			TxState.frameCounter++;
			status = SUCCESS;
			break;
		}else{
			sys_info_print("send retry:", 11);
		}
    send_timeout++;
	}
	
	set_awake_time_after_send(100);
	tag_state.sleepflag	= TRUE;
	//if(status == ERROR) Set_LedOn(LED_RED);
	return status;
}

uint8 wake_up_msg_check(uint8* data, uint16 panid, uint16 short_addr)
{
	pwake_header_t header = (pwake_header_t)data;
   
	if(FCF_WAKE_UP_DATA == header->head && panid == header->panid)
	{
		int i = 0; 
		uint16* pdata = (uint16*)(data + sizeof(wake_header_t));
		for(i=0;i< header->tag_cnt; i++)
		{
			if(*pdata == short_addr)
			{
				return 0;
			}
			pdata++;
		}
	}
   return 1;
}

void buss_pkt_handle(uint8 *pkt_tmp, uint8 packetLength, uint8 rssi)
{
	// It is assumed that the radio rejects packets with invalid length.
     	// Subtract the number of bytes in the frame overhead to get actual payload.
    	uint8 dst_addr_mode = 0;
    	uint8 src_addr_mode = 0;
    	uint8 seqNumber = 0;
     	uint8 index = 0;

    	if(RxMpdu.MpduNum < MAX_MPDU_RXBUFF_NUM)
    	{
       	index = (RxMpdu.CurrentMpdu + RxMpdu.MpduNum)%MAX_MPDU_RXBUFF_NUM;
     	}
     	else
     	{
      	return;
      }
      
			
      RxMpdu.MpduBuff[index].MpduPayload[0] = packetLength;
			
      memcpy(&RxMpdu.MpduBuff[index].MpduPayload[1], pkt_tmp + 1, packetLength);
			RxMpdu.MpduBuff[index].fcf = ((uint16)RxMpdu.MpduBuff[index].MpduPayload[2]<<8) + ((uint16)RxMpdu.MpduBuff[index].MpduPayload[1]);
      dst_addr_mode = (RxMpdu.MpduBuff[index].fcf&FCF_DST_ADDR_MODE_MASK)>>FCF_DST_ADDR_MODE_OFFSET;
      src_addr_mode = (RxMpdu.MpduBuff[index].fcf&FCF_SRC_ADDR_MODE_MASK)>>FCF_SRC_ADDR_MODE_OFFSET;
        	
			// Read the source address
      if(src_addr_mode==SHORT_ADDR)
      {
        if(dst_addr_mode==LONG_ADDR){
        	RxMpdu.MpduBuff[index].srcAddr = (uint16)RxMpdu.MpduBuff[index].MpduPayload[14] + ((uint16)RxMpdu.MpduBuff[index].MpduPayload[15]<<8);
        	RxMpdu.MpduBuff[index].ApduLen = packetLength - BASIC_RF_PACKET_LONGHEAD_SIZE;
        	RxMpdu.MpduBuff[index].pApduPayload = RxMpdu.MpduBuff[index].MpduPayload + BASIC_RF_HDR_SIZE + 6;
        }else{
        	uint16 dstAddr = (uint16)RxMpdu.MpduBuff[index].MpduPayload[6] + ((uint16)RxMpdu.MpduBuff[index].MpduPayload[7]<<8);
          uint16 srcAddr = (uint16)RxMpdu.MpduBuff[index].MpduPayload[8] + ((uint16)RxMpdu.MpduBuff[index].MpduPayload[9]<<8);
        		
          RxMpdu.MpduBuff[index].srcAddr = srcAddr;
          RxMpdu.MpduBuff[index].ApduLen = packetLength - BASIC_RF_PACKET_SHORTHEAD_SIZE;
          RxMpdu.MpduBuff[index].pApduPayload = RxMpdu.MpduBuff[index].MpduPayload + BASIC_RF_HDR_SIZE;
					
					if((tag_flash_info.gateway_addr != BROADCAST) && (srcAddr != tag_flash_info.gateway_addr))
          {
              return;
          }
                        
          if(dstAddr==BROADCAST || tag_flash_info.short_addr != dstAddr)
        	{
        		  return;
        	}
        }
      }
      else
    	{
    	  return;
      }

    
      seqNumber = RxMpdu.MpduBuff[index].MpduPayload[3];
      
			/*add rx pkg*/
			RxMpdu.MpduBuff[index].seqNumber = seqNumber;
      RxMpdu.MpduNum++;


}

void wakeup_pkt_handle(uint8 *pkt_tmp, uint8 packetLength, uint8 rssi)
{
	if(0 == wake_up_msg_check(pkt_tmp, tag_flash_info.pan_id, tag_flash_info.short_addr))
	{
		system_flag.sys_wake_up = 1;
	}
}
void frame_pkt_handle(uint8 *pkt_tmp, uint8 pkt_len, uint8 rssi)
{
  uint8 packetLength;
	uint8 tail;
	
	uint16 head = (((uint16)*(pkt_tmp+2))<< 8) + ((uint16)*(pkt_tmp+1));

	packetLength = *pkt_tmp;
	packetLength &= BASIC_RF_PLD_LEN_MASK;
	tail = *(pkt_tmp + pkt_len);
	
  /*check TAIL*/
  if(tail != TAIL) return;
	
	if(tag_state.state == LISTEN_RUN && FCF_WAKE_UP_DATA == head) 
	{
		
		wakeup_pkt_handle(pkt_tmp, packetLength, rssi);
		return;
	}
	
	buss_pkt_handle(pkt_tmp, packetLength, rssi);
	
	return;

}



