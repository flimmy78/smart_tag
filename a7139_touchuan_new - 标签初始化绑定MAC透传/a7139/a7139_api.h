
#ifndef _A7139_API_H
#define _A7139_API_H

#define FRAME_PKT_LEN    64

#define ERR_PARAM 			0x01
#define ERR_PLATFORM		0x02
#define ERR_UNK			0x03
#define ERR_CAL			0x04
#define ERR_TMO			0xFF		
#define ERR_RCOSC_CAL		0x04
#define OK_RCOSC_CAL		0x05
#define ERR_GET_RSSI		0x00

typedef enum {ReSet = 0, Set = !ReSet} RF_STATE;

/*RF CHAN definition*/
#define FREQ_433_BUGL  0x0021D405//0x0021F7D1 //0x0021D805
#define FREQ_433_JOIN  0x0021E66B
#define FREQ_433_BUSS  0x0021F7D1//0x0021D405 //0x0021F205

uint8 A7139_SetChn(uint8 chn);

void a7139_recv_handle(void);

void InitRF(void);
#endif


