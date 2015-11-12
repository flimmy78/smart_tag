
#ifndef _EP12864_H
#define _EP12864_H

#include "main.h"
//data,  LCD PIN 12
//clk,   LCD PIN 11
//reset, LCD PIN 10
//cs,    LCD PIN 9
// display\control data, LCD PIN 8

#define	LCD_COL_MAX 	 128
#define	LCD_LINE_MAX      64
#define	LCD_PAG_MAX 	   8

void Init_EPaper(uint8 repeat_times);

void update_disp_screen(void);

void Write_EPaper_GUI(uint8 pag,uint8 col,uint8 wid,uint8 hig, uint8 *ARR);

void Set_Block(uint8  hor_st,
               uint8  col, 
               uint8  wid , 
               uint8  hig ,
               uint8 graph, 
               uint8 *background_graph);

void Graph_Change(uint8  hor_st,
            	  uint8  col, 
            	  uint8  wid , 
                  uint8  hig ,
                  uint8 *graph, 
                  uint8 *background_graph);

void Write_EPaper_RAM( uint8 *ARR,uint8 color);


#endif


