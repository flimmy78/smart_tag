/***********************************************************************************
  Filename:     basic_rf.c

  Description:  Basic RF library

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "basic_rf.h"

#include "string.h"

/***********************************************************************************
* GLOBAL VARIABLES
*/
Mpdu_TxBuff_t TxMpdu; //for sender
Mpdu_RxBuff_t RxMpdu; //for receiver
BasicRfTxState_t TxState;
BasicRfCfg_t tagRfConfig;

/************ ***********************************************************************
* LOCAL FUNCTIONS
*/
/***********************************************************************************
* @fn          basicRfRxFrmDoneIsr
*
* @brief       Interrupt service routine for received frame from radio
*              (either data or acknowlegdement)
*
* @param       rxi - file scope variable info extracted from the last incoming
*                    frame
*              txState - file scope variable that keeps tx state info
*
* @return      none
*/
static void basicRfRxFrmDoneIsr(void)
{
    uint8 *pStatusWord;
    uint8 packetLength = 0;

    // Clear interrupt and disable new RX frame done interrupt
    halRfDisableRxInterrupt();

    // Enable all other interrupt sources (enables interrupt nesting)
    halIntOn();

    // Read payload length.
    halRfReadRxBuf(&packetLength,1);
    packetLength &= BASIC_RF_PLD_LEN_MASK; // Ignore MSB
    
    // Is this an acknowledgment packet?
    // Only ack packets may be 5 bytes in total.
    if (packetLength == BASIC_RF_ACK_PACKET_SIZE)
    {
        // Read the packet
    	uint8 seqNumber = 0;
    	uint16 fcf = 0;
        uint8 frametype = 0;
        uint8 AckBuff[10];

        AckBuff[0] = packetLength;
        halRfReadRxBuf(&AckBuff[1], packetLength);
        fcf = ((uint16)AckBuff[2]<<8)+((uint16)AckBuff[1]);
        frametype = (uint8)(fcf&FCF_TYPE_MASK);
        if(frametype==TYPE_ACK)
        {
            // Read the status word and check for CRC OK
            pStatusWord= AckBuff + 4;
            seqNumber = AckBuff[3];
            // Indicate the successful ACK reception if CRC and sequence number OK
            if ((pStatusWord[1] & BASIC_RF_CRC_OK_BM)
            	&& (seqNumber == TxMpdu.txSeqNumber))
            {
                TxState.ackReceived = TRUE;
            }
        }
        // No, it is data
    }
    else
    {
        // It is assumed that the radio rejects packets with invalid length.
        // Subtract the number of bytes in the frame overhead to get actual payload.
    	uint8 dst_addr_mode = 0;
    	uint8 src_addr_mode = 0;
    	uint8 seqNumber = 0;
        uint8 index = 0;
        uint8 rssi = 0;
        uint8 rxbuf[128];

        if(RxMpdu.MpduNum < MAX_MPDU_RXBUFF_NUM)
        {
        	index = (RxMpdu.CurrentMpdu + RxMpdu.MpduNum)%MAX_MPDU_RXBUFF_NUM;
        }
        else
        {
        	halRfReadRxBuf(rxbuf, packetLength);
        	return;
        }

        RxMpdu.MpduBuff[index].MpduPayload[0] = packetLength;
        halRfReadRxBuf(&RxMpdu.MpduBuff[index].MpduPayload[1], packetLength);
        RxMpdu.MpduBuff[index].fcf = ((uint16)RxMpdu.MpduBuff[index].MpduPayload[2]<<8)
        				+((uint16)RxMpdu.MpduBuff[index].MpduPayload[1]);
        dst_addr_mode = (RxMpdu.MpduBuff[index].fcf&FCF_DST_ADDR_MODE_MASK)>>FCF_DST_ADDR_MODE_OFFSET;
        src_addr_mode = (RxMpdu.MpduBuff[index].fcf&FCF_SRC_ADDR_MODE_MASK)>>FCF_SRC_ADDR_MODE_OFFSET;
        // Read the source address
        if(src_addr_mode==SHORT_ADDR)
        {
        	if(dst_addr_mode==LONG_ADDR)
        	{
        		RxMpdu.MpduBuff[index].srcAddr
        		= (uint16)RxMpdu.MpduBuff[index].MpduPayload[14]
        		  +((uint16)RxMpdu.MpduBuff[index].MpduPayload[15]<<8);
        		RxMpdu.MpduBuff[index].ApduLen = packetLength - BASIC_RF_PACKET_LONGHEAD_SIZE;
        		RxMpdu.MpduBuff[index].pApduPayload
        		= RxMpdu.MpduBuff[index].MpduPayload + BASIC_RF_HDR_SIZE + 6;
        	}
        	else
        	{
        		uint16 dstAddr = (uint16)RxMpdu.MpduBuff[index].MpduPayload[6]
        		                 +((uint16)RxMpdu.MpduBuff[index].MpduPayload[7]<<8);
                        uint16 srcAddr = (uint16)RxMpdu.MpduBuff[index].MpduPayload[8]
        		  +((uint16)RxMpdu.MpduBuff[index].MpduPayload[9]<<8);
        		
                        RxMpdu.MpduBuff[index].srcAddr = srcAddr;
        		RxMpdu.MpduBuff[index].ApduLen = packetLength - BASIC_RF_PACKET_SHORTHEAD_SIZE;
        		RxMpdu.MpduBuff[index].pApduPayload
        		= RxMpdu.MpduBuff[index].MpduPayload + BASIC_RF_HDR_SIZE;
                        
                        if((tagRfConfig.gw_addr != BROADCAST) && (srcAddr != tagRfConfig.gw_addr))//过滤源地址
                        {
                            return;
                        }
                        
                        if(dstAddr==BROADCAST)//过滤广播数据
        		{
        		    return;
        		}
        	}
        }
    	else
    	{
    		return;
    	}

        // Read the FCS to get the RSSI and CRC
        pStatusWord = RxMpdu.MpduBuff[index].pApduPayload + RxMpdu.MpduBuff[index].ApduLen;
        rssi = pStatusWord[0];
        if(rssi < 128)
        {
        	RxMpdu.MpduBuff[index].rssi = rssi - halRfGetRssiOffset();
        }
        else
        {
        	RxMpdu.MpduBuff[index].rssi = (rssi - 256) - halRfGetRssiOffset();
        }
        seqNumber = RxMpdu.MpduBuff[index].MpduPayload[3];
        // Notify the application about the received data packet if the CRC is OK
        // Throw packet if the previous packet had the same sequence number
        if(pStatusWord[1] & BASIC_RF_CRC_OK_BM)
        {
        	RxMpdu.MpduBuff[index].seqNumber = seqNumber;
        	RxMpdu.MpduNum++;
        }
    }
  
    // Enable RX frame done interrupt again
    halIntOff();
    halRfEnableRxInterrupt();
}


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn          basicRfInit
*
* @brief       Initialise basic RF datastructures. Sets channel, short address and
*              PAN id in the chip and configures interrupt on packet reception
*
* @param       pRfConfig - pointer to BASIC_RF_CONFIG struct.
*                          This struct must be allocated by higher layer
*              txState - file scope variable that keeps tx state info
*              rxi - file scope variable info extracted from the last incoming
*                    frame
*
* @return      none
*/
static void halRfDataInit(void)
{
	uint8 i;
	memset(&TxMpdu,0,sizeof(Mpdu_TxBuff_t));
	memset(&RxMpdu,0,sizeof(Mpdu_RxBuff_t));
	for(i=0;i<MAX_MPDU_RXBUFF_NUM;i++)
	{
		RxMpdu.MpduBuff[i].pApduPayload = NULL;
	}
	memset(&tagRfConfig,0,sizeof(BasicRfCfg_t));
	memset(&TxState,0,sizeof(BasicRfTxState_t));
}
uint8 basicRfInit(void)
{
    halRfDataInit();
    if (halRfInit()==FAILED)
    {
        return FAILED;
    }
    halIntOff();
    
    // Set the protocol configuration
    memcpy(tagRfConfig.IEEEAddr,(uint8*)HAL_STORE_IEEE_ADDR_LOCATION,8);
    halRfSetIEEEAddr(tagRfConfig.IEEEAddr);
   
    // Set channel
    tagRfConfig.channel = RF_DEFAULT_CHANNEL;
    halRfSetChannel(tagRfConfig.channel);

    // Write the short address and the PAN ID to the CC2530 RAM
    tagRfConfig.shortAddr = DEFAULT_SHORTADDR;
    halRfSetShortAddr(tagRfConfig.shortAddr);
    
    tagRfConfig.panId = DEFAULT_PANID;
    halRfSetPanId(tagRfConfig.panId);

    // Set up receive interrupt (received data or acknowlegment)
    halRfRxInterruptConfig(basicRfRxFrmDoneIsr);

    halIntOn();

    return SUCCESS;
}

static uint8 halRfBuildHeader(pMpdu_TxBuff_t pTxMpdu)
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
        	pTxMpdu->MpduPayload[8] = tagRfConfig.IEEEAddr[0];
        	pTxMpdu->MpduPayload[9] = tagRfConfig.IEEEAddr[1];
        	pTxMpdu->MpduPayload[10] = tagRfConfig.IEEEAddr[2];
        	pTxMpdu->MpduPayload[11] = tagRfConfig.IEEEAddr[3];
        	pTxMpdu->MpduPayload[12] = tagRfConfig.IEEEAddr[4];
        	pTxMpdu->MpduPayload[13] = tagRfConfig.IEEEAddr[5];
        	pTxMpdu->MpduPayload[14] = tagRfConfig.IEEEAddr[6];
        	pTxMpdu->MpduPayload[15] = tagRfConfig.IEEEAddr[7];
            return 16;
        }
        else
        {
        	pTxMpdu->MpduPayload[0] = pTxMpdu->ApduLen + BASIC_RF_PACKET_SHORTHEAD_SIZE;
        	pTxMpdu->MpduPayload[8] = LO_UINT16(tagRfConfig.shortAddr);
        	pTxMpdu->MpduPayload[9] = HI_UINT16(tagRfConfig.shortAddr);
            return 10;
        }
    }
    else
    {
    	return 0;
    }
}

static uint8 halRfBuildMpdu(pMpdu_TxBuff_t pTxMpdu)
{
    uint8 hdrLength, n;

    hdrLength = halRfBuildHeader(pTxMpdu);

    for(n=0;n<pTxMpdu->ApduLen;n++)
    {
    	pTxMpdu->MpduPayload[hdrLength+n] = pTxMpdu->ApduPayload[n];
    }
    return hdrLength + pTxMpdu->ApduLen; // total mpdu length
}
/***********************************************************************************
* @fn          basicRfSendPacket
*
* @brief       Send packet
*
* @param       destAddr - destination short address
*              pPayload - pointer to payload buffer. This buffer must be
*                         allocated by higher layer.
*              length - length of payload
*              txState - file scope variable that keeps tx state info
*              mpdu - file scope variable. Buffer for the frame to send
*
* @return      basicRFStatus_t - SUCCESS or FAILED
*/
static uint8 basicRfTransmitPacket(void)
{
    uint8 status = 0;
    uint8 length = 0;
    bool AckRequest = FALSE;

    // Turn on receiver if its not on [receiver is a moduler and shall turn up.]
    if(!TxState.receiveOn)
    {
        halRfReceiveOn();
    }
    length = halRfBuildMpdu(&TxMpdu); //pack TxMpdu struct.
    // Check packet length
    if(length>127)
    {
    	return FAILED;
    }

    // Wait until the transceiver is idle
    halRfWaitTransceiverReady();

    txCsmaPrep(); //Prepare/initialize for a CSMA transmit.

    // Turn off RX frame done interrupt to avoid interference on the SPI interface
    halRfDisableRxInterrupt();

    halRfWriteTxBuf(TxMpdu.MpduPayload, length); //Write to TX buffer

    // Turn on RX frame done interrupt for ACK reception
    halRfEnableRxInterrupt();

    txGo();

//    // Send frame with CCA. return FAILED if not successful
//    if(halRfTransmit() != SUCCESS)
//    {
//        status = FAILED;
//    }

    AckRequest = (TxMpdu.fcf&FCF_ACK_REQUEST_MASK)>>FCF_ACK_REQUEST_OFFSET;
    // Wait for the acknowledge to be received, if any
    if (AckRequest==TRUE) {
        // We'll enter RX automatically, so just wait until we can be sure that the ack reception should have finished
        // The timeout consists of a 12-symbol turnaround time, the ack packet duration, and a small margin
        uint16 i=0;
    	while((i++)<60000) //60ms may be enough for a cycle.
    	{
    	    halMcuWaitUs(1);
            // If an acknowledgment has been received (by RxFrmDoneIsr), the ackReceived flag should be set
            status = TxState.ackReceived ? SUCCESS : FAILED;
            if(status==SUCCESS)
            {
            	break;
            }
    	}

    	//halMcuWaitUs((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) + (2 * BASIC_RF_SYMBOL_DURATION) + 8000);
        TxState.ackReceived = FALSE;

    } else {
        status = SUCCESS;
    }

    // Turn off the receiver if it should not continue to be enabled
    if (!TxState.receiveOn) {
        halRfReceiveOff();
    }

    if(status == SUCCESS) {
        TxState.frameCounter++;
    }

    return status;
}

//rf send packet to gw.and try retransmit times.
int8 basicRfSendPacket(void)
{
	uint8 i=0;
        int8 sendStatus = TRANS_FAILED;
        
	while((i++)<TIMEOUT_RETRANSMIT)
	{
                WD_KICK();
                
		if(basicRfTransmitPacket()==SUCCESS)
		{
                        sendStatus = TRANS_SUCCESS;
			break;
		}
		else
		{
			TxState.ackReceived = FALSE;
		}
	}
        return sendStatus;
}

/***********************************************************************************
* @fn          basicRfPacketIsReady
*
* @brief       Check if a new packet is ready to be read by next higher layer
*
* @param       none
*
* @return      uint8 - TRUE if a packet is ready to be read by higher layer
*/
uint8 basicRfPacketIsReady(void)
{
	if(RxMpdu.MpduNum!=0)
	{
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}
}

/***********************************************************************************
* @fn          basicRfReceiveOn
*
* @brief       Turns on receiver on radio
*
* @param       txState - file scope variable
*
* @return      none
*/
void basicRfReceiveOn(void)
{
    TxState.receiveOn = TRUE;
    halRfReceiveOn();
}


/***********************************************************************************
* @fn          basicRfReceiveOff
*
* @brief       Turns off receiver on radio
*
* @param       txState - file scope variable
*
* @return      none
*/
void basicRfReceiveOff(void)
{
    TxState.receiveOn = FALSE;
    halRfReceiveOff();
}
