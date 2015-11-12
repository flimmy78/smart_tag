#include "crc16.h"

uint16_t crc_step(uint16_t crc, uint8_t data) {
	uint8_t v = 0x80;
	uint8_t bit;
	uint8_t xor_flag;

	for (bit = 0; bit < 8; bit++) {
		if (crc & 0x8000)
			xor_flag = 1;
		else 
			xor_flag = 0;

		crc <<= 1;

		if (data & v) 
			crc += 1;

		if (xor_flag)
			crc ^= CRC16_POLY_VAL;
		
		v >>= 1;
	}

	return crc;
}

uint16_t crc_finish(uint16_t crc) {
	uint8_t bit;
	uint8_t xor_flag;

	for(bit = 0; bit < 16; bit++) {
    	if(crc & 0x8000)
        	xor_flag= 1;
    	else
        	xor_flag= 0;

		crc <<= 1;

    	if(xor_flag)
			crc ^= CRC16_POLY_VAL;
	}

	return crc;
}

uint16_t cal_crc16(uint8_t *data, uint16_t len) {
	uint16_t i;
	uint16_t crc = 0xFFFF;

	for (i = 0 ; i < len; i++) 
		crc = crc_step(crc, data[i]);

	crc = crc_finish(crc);

	return crc;
}
