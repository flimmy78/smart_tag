/*
 * app_wrscreen.h
 */

#ifndef APP_WRSCREEN_H_
#define APP_WRSCREEN_H_
#include "app_typedef.h"

#pragma pack (1)
//for description of data to Screen


typedef struct
{
  uint8  Coords_X;  
	uint8  Coords_Y;
	uint8  Image_Width;
  uint8  Image_High;
  uint8  blockid;
	uint8  screen_id;
  uint8  displayflag;
	uint16 data_Lenth;
}template_t, *ptemplate_t;

typedef struct
{
	uint8 screen_id;
	uint8  charblockid;
	uint8  numblockid;
	uint32 integ_part;
	uint8  dec_part;
	uint8  unit_show_type;
	uint8  ers_flag;
}lite_screen_t,*plite_screen_t;

#define LEN_EXCEPT_PAYLOAD_DATA 8
typedef struct
{
	uint8  blockid;
  uint8  screen_id;	
	uint8  Coords_X;  
	uint8  Coords_Y;
	uint8  Image_Width;
	uint8  Image_High;
	uint8  displayflag;
	uint16 data_Lenth;
	uint8  data[MAX_BLOCK_LENGTH];
}cell_data_t,*pcell_data_t; //display block data structure.



#pragma pack()


extern block_idx_info_t block_idx_info;

void fill_disp_buffer(uint8* data_p,uint8 *back_p, bool save, bool screen_function_active, uint8 num_block_offset_black);
void fill_screen_buffer(uint8 *back_p);
void Erase_Indic_block(uint8 *back_p);

void Write_EPaper(uint8 *data_p, uint8 *back_p);
void refresh_screen(uint8 *back_p);


#endif /* APP_WRSCREEN_H_ */
