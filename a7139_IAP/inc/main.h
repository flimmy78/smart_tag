
#ifndef _MAIN_H
#define _MAIN_H
#include "stm32f0xx.h"
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef unsigned char   bool;


typedef  void (*pFunction)(void);


#define STORE_SYS_INFO_PAGE_ADDR   0x0800FC00
#define MAX_CELL_DATA             ((uint8)35)

#define SYS_PARA_MAGIC_NUM        ((uint16)0xA1A4)


#define QRCODE
#pragma pack (1)
typedef struct
{
	uint8 blockid;
	uint8 flash_page;
	uint16 flash_offset;
	uint16 blocksize;
}idx_table_t,*pidx_table_t;//display block storing indexer.

typedef struct
{
	uint16 net_flag_set;
	uint16 pan_id;
	uint16 short_addr;
	uint16 gateway_addr;
	uint16 listen_channel;
	uint16 work_channel;
	
	uint16 update_software_flag;
	uint16 software_crc;
	
	uint16 block_idx_flag_set;
	uint8  block_num;
	idx_table_t idx_tb[MAX_CELL_DATA];
#ifdef QRCODE
	uint16 qr_set_flag;
	uint16 qr_crc;
	uint8  qrcode[63];
#endif
}tag_flash_info_t, *ptag_flash_info_t; //table of display block storing indexer.

#pragma pack ()

#endif
