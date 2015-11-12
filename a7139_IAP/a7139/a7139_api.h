
#ifndef _A7139_API_H
#define _A7139_API_H

#include "main.h"


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

typedef struct {
	RF_STATE rf_data_recv_state;
	RF_STATE rf_data_send_state;
} a7139_rf_t;


extern a7139_rf_t rf_state;

void a7139_recv_handle(void);

#endif


