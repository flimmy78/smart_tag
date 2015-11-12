
#include "bsp.h"

static void data_out(uint8 dat);


static void com_out(uint8 cmd)
{
	uint8 i;
	EP12864_IO_RstBit(RS);
	EP12864_IO_RstBit(SCLK);
	EP12864_IO_RstBit(CS);
  __ASM volatile ("nop");
	__ASM volatile ("nop");
	
	for(i=0;i<8;i++)
	{
	  EP12864_IO_RstBit(SCLK);
	        __ASM volatile ("nop");

          if((cmd&0x80)==0x80)
	        EP12864_IO_SetBit(SID);
          else
                EP12864_IO_RstBit(SID); 
	  cmd=cmd<<1;
	        __ASM volatile ("nop");
 
	  EP12864_IO_SetBit(SCLK);
	        __ASM volatile ("nop");
          __ASM volatile ("nop");
	}
	EP12864_IO_SetBit(CS);
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        
	return;
}

static void Repeat_set (unsigned char repeat_times1, unsigned char repeat_times2, unsigned char repeat_times3)
{
    com_out (0x93);                   //VA clear repeat times
    com_out (repeat_times1);    
    
    com_out (0x94);                   //VA idle repeat times                               
    com_out (repeat_times1);

    com_out (0x95);                   //AA clear repeat times
    com_out (repeat_times2);

    com_out (0x96);                   //AA idle repeat times
    com_out (repeat_times2);

    com_out (0x97);                   //Driving repeat times 
    com_out (repeat_times3);            
}

void Init_EPaper(uint8 repeat_times)
{
	EP12864_IO_RstBit(RES);
	Delay100us(50);
	EP12864_IO_SetBit(RES);
   
  __ASM volatile ("nop");
	__ASM volatile ("nop");
	com_out(0xe9);     //
	com_out(0x84);     // Enable bias ladder

	com_out(0x80);	   // Set the non TC value
	com_out(0x00);     // Set temperature value
	com_out(0x09);     // Set VA clear hold time  16    12
	com_out(0x03);     // Set VA idle  hold time   03
	com_out(0x09);     // Set AA clear hold time  16    12
	com_out(0x03);     // Set AA idle  hold time   03
	com_out(0x0a);//com_out(0x0f);     // Set driving using time  13
	com_out(0x58);     // 30V
	com_out(0x58);     // 30V

	Repeat_set (0, 0, repeat_times);

	com_out(0x32);
	com_out(0x00);     // Set scheme B

	com_out(0xA3);
	com_out(0x1A);	   // *Enable band gap + other analog control
	
	com_out(0x2f);     // set power control register
 
	com_out(0xF6);
	com_out(0x40);	   // *Enable Oscillator

	com_out(0xa8);
	com_out(0x40);     // Set duty = 1/64

	com_out(0xa2);
	com_out(0x00);     // Set bias = 1/9

	com_out(0xa7);	   //背景白色

// com_out(0xa1);     // Set segment re-map = 0 -> 127
	com_out(0xa0);     // Set segment re-map

	com_out(0xc8);     // Set common re-map = 63 -> 0
// com_out(0xc7);

	com_out(0xb0);     // set page address

	com_out(0x40);     // set display start line

	com_out(0xd3);
	com_out(0x00);     // display offset
        
	com_out(0xad);
	com_out(0x00);     // Horizontal mode

	com_out(0x10);     // set higher column address
	com_out(0x00);     // set lower column address

	//com_out(0x31);  //update by cyz

}

void lcd_sleep_mode(void)
{ 
   com_out (0xE9);
   com_out (0x04);
	
   com_out (0xA3);
   com_out (0x00);
	
   com_out (0xA9);
   com_out (0x00);
    
   com_out (0x2A);     // set power control register,turns off charge pump and bias voltage buffer
	
   com_out (0xF6);    // *DISable Oscillator
   com_out (0x00);      
     
   com_out(0x31);
}


void update_disp_screen()
{   com_out(0x31); 
    while(EP12864_IS_BUSY());
    lcd_sleep_mode();
}

static void Set_Page_Address(uint8 page)
{
	page=0xb0|page;
	com_out(page);
	return;
}
/**************************************************************
//设置列地址
**************************************************************/  
static void Set_Column_Address(uint8 col)
{
	com_out((0x10|(col>>4))); //qugao4
	com_out((0x0f&col));      //qudi 4
	return;
}		 

static void data_out(uint8 dat)
{									   
	uint8 i;
	EP12864_IO_SetBit(RS); 
	EP12864_IO_RstBit(SCLK); 
	EP12864_IO_RstBit(CS); 
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	for(i=0;i<8;i++)
	{
		EP12864_IO_RstBit(SCLK);
		__ASM volatile ("nop");

                if((dat&0x80)==0x80)
		EP12864_IO_SetBit(SID);
                else
                EP12864_IO_RstBit(SID); 
		dat=dat<<1;
	
                __ASM volatile ("nop");
	
		EP12864_IO_SetBit(SCLK);
		__ASM volatile ("nop");
	}
	EP12864_IO_SetBit(CS);
	__ASM volatile ("nop");
	__ASM volatile ("nop");
   
	return;
}

void Clear_EPaper(void)
{	
  uint8 i,j;
  Set_Column_Address(0);
		 
      for(j=0;j<8;j++)
     { 
        Set_Page_Address(j);;     //SET PAG
	   	 
	  for(i=0;i<64;i++)
	   {
	   data_out(0); //2?,?512?
	   data_out(0);
	   }  
	   data_out(0x00);
	   data_out(0x00);
	   data_out(0x00);
	   data_out(0x00);    
	  }
	 com_out(0x31);
		while(EP12864_IS_BUSY());
   //while(0x08 & P1) //wait utill refresh e-paper job done ok.

}
void Write_EPaper_GUI(uint8 pag,uint8 col,uint8 wid,uint8 hig, uint8 *ARR)
{	
     uint8 i,j,t;
     
     t=hig/8+pag;

     for(j=pag;j<t;j++)
     { 
          Set_Page_Address(j);

	  Set_Column_Address(col);
	 
	  for(i=0;i<wid;i++){
                  data_out(*ARR++);   
		   __ASM volatile ("nop");
		   __ASM volatile ("nop");
		   __ASM volatile ("nop");
          }   	  
     }
     
     //delayms(5);
     com_out(0x31);   
    __ASM volatile ("nop");
   __ASM volatile ("nop");
   __ASM volatile ("nop");
  
     while(EP12864_IS_BUSY());
     lcd_sleep_mode();
}


/**************************************************************

**************************************************************/	  
void Set_Block( uint8  hor_st,
            uint8  col, 
            uint8  wid , 
            uint8  hig ,
            uint8 graph, 
            uint8 *background_graph )
  {	
     uint8 i,j,h,t,v;
     uint8  pag;
     uint8 *ARR;
     ARR = background_graph;

     pag=hor_st/8;                  
                        
     t=hig/8;
     v=(hor_st+hig)%8;             
     h=8-(hor_st%8);              
     ARR=ARR+(pag*128+col);         
     
     //WD_KICK();
     
    if ( (hor_st%8)&&( pag==((hor_st+hig)/8 )) )   
      {
        for(i=0;i<wid;i++ )
        {
          *ARR++=( ( (graph)&((0xff<<(hor_st%8))& (0xff>> (8-v)))) | ( (*ARR)&( (0xff<<v)|(0xff>> h) ) ) ) ; 
        }
        return;
      } 
           
    if(hor_st%8)               
      {    
         for(i=0;i<wid;i++ )
          {      
           *ARR++=( ((graph)&(0xff<<(hor_st%8))) | ((*ARR)&(0xff>>h )) );                   
          }
         ARR=(ARR+128)-wid; 
         t=(hig-h)/8;
      }
         
    for(j=0;j< t;j++)         
      {                
        for(i=0;i<wid;i++)
        {
         
         *ARR++ = graph;
        }
        ARR=(ARR+128)-wid;         
      }
       
    if(v)             
     {           
       for(i=0;i<wid;i++ )
          {      
           *ARR++=(((graph)&(0xff>>(8-v))) | ((*ARR)&(0xff<<v )));                   
          }       
     }              
}

void Graph_Change( uint8  hor_st,//horizonal st
            uint8  col, 
            uint8  wid , 
            uint8  hig ,
            uint8 *graph, //point
            uint8 *background_graph, uint8 turn)
{	
  int t = 0;
	//uint8 data_gui[1024];
	uint8 *gui_ARR = graph;//data_gui;
	uint8 *back_ARR = background_graph;
	uint8 divide_block_cnt = hig/8;
	
	uint16 back_ops = (hor_st/8)*128 + col;
	uint16 gui_ops  = 0;
	uint8 gui_offset = hor_st%8;
	uint8 back_offset = 8 - (hor_st%8);
	
	//memcpy(data_gui, graph, hig/8*wid);
	if(turn == 1) for(t = 0;t<(hig/8*wid);t++) *(gui_ARR+t) = ~ *(gui_ARR+t);
	
	while(divide_block_cnt>0)
	{
		for(t =0; t< wid; t++)
		{
			uint8 updata 	= ((uint8)(*(back_ARR + back_ops + t)<<back_offset))>>back_offset;
			uint8 downdata = (uint8)(*(gui_ARR + gui_ops + t)<<gui_offset);
			*(back_ARR+back_ops + t) = updata | downdata;
		}
		back_ops += 128;
		
		for(t =0; t< wid; t++)
		{
			uint8 updata 	= (uint8)(*(gui_ARR + gui_ops + t)>>back_offset);
			uint8 downdata = ((uint8)(*(back_ARR + back_ops + t)>>gui_offset))<<gui_offset;
			*(back_ARR+back_ops + t) = updata | downdata;
		}
		gui_ops += wid;
		divide_block_cnt --;
	}
}
	
void Write_EPaper_RAM( uint8 *ARR,uint8 color)
{	
     uint8 i,j; 
     
     //WD_KICK();
     
     for(j=0;j<LCD_PAG_MAX;j++)
        { 
          Set_Page_Address(j);
	  Set_Column_Address(0);
          
	  for(i=0;i<LCD_COL_MAX ;i++)
	     {
	      if(color)
	      {
                  data_out(*ARR++);    
									__ASM volatile ("nop");
									__ASM volatile ("nop");
									__ASM volatile ("nop");
			  }   
	      else	   
	        {
						 data_out(~(*ARR++));  
						__ASM volatile ("nop");
									__ASM volatile ("nop");
									__ASM volatile ("nop");;
	        } 
	     }  
	  }	
}   

	







