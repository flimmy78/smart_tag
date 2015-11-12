/***********************************************************************************
  Filename:     basic_rf.h

  Description:  Basic RF library header file

***********************************************************************************/
#ifndef BASIC_RF_H
#define BASIC_RF_H

/***********************************************************************************
    The "Basic RF" library contains simple functions for packet transmission and
    reception with the IEEE 802.15.4 compliant radio devices. The intention of this
    library is to demonstrate how the radio devices are operated, and not to provide
    a complete and fully-functional packet protocol. The protocol uses 802.15.4 MAC compliant data
    and acknowledgment packets, however it contains only a small subset of  the
    802.15.4 standard:
    - Association, scanning nor beacons are not implemented
    - No defined coordinator/device roles (peer-to-peer, all nodes are equal)
    - Waits for the channel to become ready, but does not check CCA twice
    (802.15.4 CSMA-CA)
    - Does not retransmit packets

    INSTRUCTIONS:
    Startup:
    1. Create a basicRfCfg_t structure, and initialize the members:
    2. Call basicRfInit() to initialize the packet protocol.

    Transmission:
    1. Create a buffer with the payload to send
    2. Call basicRfSendPacket()

    Reception:
    1. Check if a packet is ready to be received by highger layer with
    basicRfPacketIsReady()
    2. Call basicRfReceive() to receive the packet by higher layer

    FRAME FORMATS:
    Data packets (without security):
    [Preambles (4)][SFD (1)][Length (1)][Frame control field (2)]
    [Sequence number (1)][PAN ID (2)][Dest. address (2)][Source address (2)]
    [Payload (Length - 2+1+2+2+2)][Frame check sequence (2)]

    Acknowledgment packets:
    [Preambles (4)][SFD (1)][Length = 5 (1)][Frame control field (2)]
    [Sequence number (1)][Frame check sequence (2)]
***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "app_typedef.h"
#include "app_config.h"

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

// Packet and packet part lengths
#define PKT_LEN_MIC                         8
#define PKT_LEN_SEC                         PKT_LEN_UNSEC + PKT_LEN_MIC
#define PKT_LEN_AUTH                        8
#define PKT_LEN_ENCR                        24

// Packet overhead ((frame control field, sequence number, PAN ID,
// destination and source) + (footer))
// Note that the length byte itself is not included included in the packet length
#define BASIC_RF_PACKET_LONGHEAD_SIZE       ((2 + 1 + 2 + 8 + 2) + (2))
#define BASIC_RF_PACKET_SHORTHEAD_SIZE		((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_ACK_PACKET_SIZE	        5
#define BASIC_RF_FOOTER_SIZE                2
#define BASIC_RF_HDR_SIZE                   10

// The time it takes for the acknowledgment packet to be received after the
// data packet has been transmitted.
#define BASIC_RF_ACK_DURATION		        (0.5 * 32 * 2 * ((4 + 1) + (1) + (2 + 1) + (2)))
#define BASIC_RF_SYMBOL_DURATION	        (32 * 0.5)

// The length byte
#define BASIC_RF_PLD_LEN_MASK               0x7F

// Frame control field
#define MAC_FCF   0x0
//frame type
#define TYPE_BEACON 0
#define TYPE_DATA   1
#define TYPE_ACK    2
#define TYPE_MAC_CMD 3
//dest or src addr mode
#define LONG_ADDR 3
#define SHORT_ADDR 2

#define FCF_TYPE_MASK              0x0007
#define FCF_SECURITY_MASK          0x0008
#define FCF_ACK_REQUEST_MASK	   0x0020
#define FCF_PANIDCMPR_MASK         0x0040
#define FCF_DST_ADDR_MODE_MASK     0x0C00
#define FCF_SRC_ADDR_MODE_MASK     0xC000

#define FCF_DST_ADDR_MODE_OFFSET   10
#define FCF_SRC_ADDR_MODE_OFFSET   14
#define FCF_ACK_REQUEST_OFFSET	   5

// Auxiliary Security header
#define BASIC_RF_AUX_HDR_LENGTH             5
#define BASIC_RF_LEN_AUTH                   BASIC_RF_PACKET_OVERHEAD_SIZE + \
    BASIC_RF_AUX_HDR_LENGTH - BASIC_RF_FOOTER_SIZE
#define BASIC_RF_SECURITY_M                 2
#define BASIC_RF_LEN_MIC                    8
#ifdef SECURITY_CCM
#undef BASIC_RF_HDR_SIZE
#define BASIC_RF_HDR_SIZE                   15
#endif

// Footer
#define BASIC_RF_CRC_OK_BM                  0x80

//send result
#define TX_DONE			  1
#define CHANNEL_BUSY	          2
#define ACK_NOT_RECV	          3
#define ACK_RECV		  4
/***********************************************************************************
* TYPEDEFS
*/
// The receive struct
typedef struct {
    //receive addr
    uint16 srcAddr;
    //receive frame control field
    uint16 fcf;
    //receive seqnum
    uint8 seqNumber;
    //rssi
    int16 rssi;
    //others
    uint8 ApduLen;
    uint8 MpduPayload[MAX_MPDU_LEN];
    uint8 *pApduPayload;
} BasicRfRxInfo_t, *pBasicRfRxInfo_t;

typedef struct
{
	BasicRfRxInfo_t  MpduBuff[MAX_MPDU_RXBUFF_NUM];
	uint8        	 MpduNum;
	uint8        	 CurrentMpdu;
}Mpdu_RxBuff_t, *pMpdu_RxBuff_t;

typedef struct {
    //panid
    uint16 DestPanId;
    //addr
    uint16 DestshortAddr;
    //frame control field
    uint16 fcf;
    //seqnum
    uint8 txSeqNumber;
    //others
    uint8 ApduLen;
    uint8 ApduPayload[MAX_MPDU_LEN];
    uint8 MpduPayload[MAX_MPDU_LEN];
} Mpdu_TxBuff_t, *pMpdu_TxBuff_t;

typedef struct {
    uint16 shortAddr;
    uint8 IEEEAddr[8];
    uint16 gw_addr;
    uint16 panId;
    uint8 channel;
    #ifdef SECURITY_CCM
    uint8* securityKey;
    uint8* securityNonce;
    #endif
} BasicRfCfg_t, *pBasicRfCfg_t;

typedef struct {
	uint32 frameCounter;
	uint8 sendresult;
	bool  receiveOn;
	bool  ackReceived;
} BasicRfTxState_t, *pBasicRfTxState_t;

extern Mpdu_TxBuff_t TxMpdu;
extern Mpdu_RxBuff_t RxMpdu;
extern BasicRfTxState_t TxState;
extern BasicRfCfg_t tagRfConfig;

/***********************************************************************************
* GLOBAL FUNCTIONS
*/
uint8 basicRfInit(void);
int8 basicRfSendPacket(void);
uint8 basicRfPacketIsReady(void);
void basicRfReceiveOn(void);
void basicRfReceiveOff(void);

#endif

