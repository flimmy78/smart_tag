
#ifndef _PKT_BUILD_H
#define _PKT_BUILD_H

#include "app_config.h"

#define HI_UINT16(a) (((uint16)(a) >> 8) & 0xFF)
#define LO_UINT16(a) ((uint16)(a) & 0xFF)

/* CONSTANTS AND DEFINES*/

// Packet and packet part lengths
#define PKT_LEN_MIC                         8
#define PKT_LEN_SEC                         PKT_LEN_UNSEC + PKT_LEN_MIC
#define PKT_LEN_AUTH                        8
#define PKT_LEN_ENCR                        24

// Packet overhead ((frame control field, sequence number, PAN ID,
// destination and source) + (footer))
// Note that the length byte itself is not included included in the packet length
#define BASIC_RF_PACKET_LONGHEAD_SIZE       (2 + 1 + 2 + 8 + 2)
#define BASIC_RF_PACKET_SHORTHEAD_SIZE		(2 + 1 + 2 + 2 + 2)
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


void frame_pkt_handle(uint8 *pkt_tmp, uint8 pkt_len, uint8 rssi);
ErrorStatus RfSendPacket(void);

#endif
