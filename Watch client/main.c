#include "main.h"
#include "tft.h"
#include "jianpan.h"
#include "screen.h"




void main()
{	
	//uchar sta = 0;
    /*TMOD=0x01;		 
	TR0=1;			
	TH0=0X15;		 
	TL0=0Xa0;
	ET0=1;			
	EA=1;*/
	lcd_initial();
    bl=1;	
	
//	while(1)
//	{
//		if(sta==0)
//		{
//		  dsp_single_colour(BLACK,50,50,100,100);	
//		}
//		delay_ms(1000);
//		TX_Mode(0x11);
//		sta = SPI_Read(READ_REG+STATUS);
//		if(sta&0x20)
//		{
//		  dsp_single_colour(GREEN,50,50,100,100);	
//		}
//	}	init_io();

	Begin();
	delay_ms(3800);
	Font_Test();
	Jianpan();
	  
}






/*void time0() interrupt 1
{	TH0=0X15;
	TL0=0Xa0;
	function();
	delay_ms(3800);
}  */