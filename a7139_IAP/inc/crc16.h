#ifndef __CRC16_H__
#define	__CRC16_H__



#include "main.h"

#define CRC16_POLY_VAL  	0x1021

uint16_t crc_step(uint16_t crc, uint8_t data);
uint16_t crc_finish(uint16_t crc);
uint16_t cal_crc16(uint8_t *data, uint16_t len);

uint16_t cal_modbus_rtu_crc16(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif // __CRC16_H__
