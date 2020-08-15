/*
所用硬件
GPIO:
PC6~10
PB
*/
#include "lcd.h"
#include "sys.h"
#include "delay.h"
//#include "led.h"
#include "font.h"

//显示屏重要参数
_lcd_dev lcddev;

//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//3个基础函数
//写数据
void lcd_write_data(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	GPIOB->ODR = data;
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
//读数据
u16 lcd_read_data()
{
	u16 data;
	
	GPIOB->CRL = 0X88888888;
	GPIOB->CRH = 0X88888888;
	GPIOB->ODR = 0X0000;
	
	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_RD_CLR;
	data = GPIOB->IDR;
	LCD_RD_SET;
	LCD_CS_SET;
	
	GPIOB->CRL = 0X33333333;
	GPIOB->CRH = 0X33333333;
	GPIOB->ODR = 0XFFFF;
	
	return data;
}
//写命令
void lcd_write_reg(u16 data)
{
	LCD_RS_CLR;
	LCD_CS_CLR;
	GPIOB->ODR = data;
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}

//写操作
void lcd_write(u16 cmd,u16 data)
{
	lcd_write_reg(cmd);
	lcd_write_data(data);
}
//读操作
u16 lcd_read(u16 cmd)
{
	lcd_write_reg(cmd);
	return lcd_read_data();
}
//检查显示屏ID
u8 lcd_check()
{
	u16 id;
	u8 ck1;
	u8 ck2;
		
	lcd_write_reg(0xD3);
	lcd_read_data();
	lcd_read_data();
	ck1 = lcd_read_data();
	ck2 = lcd_read_data();
//	LED0 = 0;
	if(ck1 == 0x93)
	{
		if(ck2 == 0x41)
		{
			return 0;
		}
		return 1;
	}
	return 1;
}
//准备写GRAM
void write_ram_prepare()
{
	lcd_write_reg(lcddev.wramcmd);	
}
//写RAM
void write_ram(u16 data)
{
	lcd_write_data(data);
}
//存储器读写方向控制
void lcd_scan_dir()
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	
		regval|=(0<<7)|(0<<6)|(0<<5); 
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510不需要BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		lcd_write(dirreg,regval);
 		if(regval&0X20)
		{
			if(lcddev.width<lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}
		else  
		{
			if(lcddev.width>lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}  
			lcd_write_reg(lcddev.setxcmd); 
			lcd_write_data(0);lcd_write_data(0);
			lcd_write_data((lcddev.width-1)>>8);lcd_write_data((lcddev.width-1)&0XFF);
			lcd_write_reg(lcddev.setycmd); 
			lcd_write_data(0);lcd_write_data(0);
			lcd_write_data((lcddev.height-1)>>8);lcd_write_data((lcddev.height-1)&0XFF);  
} 
//光标设置函数
void lcd_setcursor(u16 x,u16 y)
{
	lcd_write_reg(lcddev.setxcmd);
	lcd_write_data(x>>8);
	lcd_write_data(x&0xff);
	lcd_write_reg(lcddev.setycmd);
	lcd_write_data(y>>8);
	lcd_write_data(y&0xff);
}
//设置屏幕颜色
void lcd_setcorlor(u16 color)
{
	u16 x;
	u16 y;
	lcd_setcursor(0,0);
	write_ram_prepare();
	for(y=0;y<320;y++)
	{
		for(x=0;x<240;x++)
		{
			write_ram(color);
		}
	}
}
//画线
void draw_line(u16 x1,u16 y1,u16 x2,u16 y2)
{
	u16 x;
	u16 y;

	x = x2 - x1;
	y = y2 - y1;

	if(x == 0)//画垂直线
	{
		for(x=0;x<y;x++)
		{
			lcd_setcursor(x1,y1+x);
			write_ram_prepare();
			write_ram(POINT_COLOR);
		}
	}	
	if(y == 0)//画水平线
	{
		for(y=0;y<x;y++)
		{
			lcd_setcursor(x1+y,y1);
			write_ram_prepare();
			write_ram(POINT_COLOR);
		}
	}
}

//写数字和英文的函数
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	
	lcd_setcursor(x,y);
	
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	GPIOB->ODR = lcddev.wramcmd;//写命令
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	lcd_write_data(color);		//写数据
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	
}
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
//初始化函数
void lcd_init()
{
 	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟

	RCC->APB2ENR|=1<<0;    //开启辅助时钟
	JTAG_Set(SWD_ENABLE);  //开启SWD
											 
	//PORTC6~10复用推挽输出 	
	GPIOC->CRH&=0XFFFFF000;
	GPIOC->CRH|=0X00000333; 
	GPIOC->CRL&=0X00FFFFFF;
	GPIOC->CRL|=0X33000000;  
	GPIOC->ODR|=0X07C0; 	 
	//PORTB 推挽输出 	
	GPIOB->CRH=0X33333333;
	GPIOB->CRL=0X33333333; 	 
	GPIOB->ODR=0XFFFF;
	
	lcd_write_reg(0xCF);  
	lcd_write_data(0x00); 		
	lcd_write_data(0xC1); 
	lcd_write_data(0X30); 
	lcd_write_reg(0xED);  
	lcd_write_data(0x64); 
	lcd_write_data(0x03); 
	lcd_write_data(0X12); 
	lcd_write_data(0X81); 
	lcd_write_reg(0xE8);  
	lcd_write_data(0x85); 
	lcd_write_data(0x10); 
	lcd_write_data(0x7A); 
	lcd_write_reg(0xCB);  
	lcd_write_data(0x39); 
	lcd_write_data(0x2C); 
	lcd_write_data(0x00); 
	lcd_write_data(0x34); 
	lcd_write_data(0x02); 
	lcd_write_reg(0xF7);  
	lcd_write_data(0x20); 
	lcd_write_reg(0xEA);  
	lcd_write_data(0x00); 
	lcd_write_data(0x00); 
	lcd_write_reg(0xC0);    //Power control 
	lcd_write_data(0x1B);   //VRH[5:0]
	lcd_write_reg(0xC1);    //Power control 		
	lcd_write_data(0x01);   //SAP[2:0];BT[3:0] 
	lcd_write_reg(0xC5);    //VCM control 
	lcd_write_data(0x30); 	 //3F
	lcd_write_data(0x30); 	 //3C
	lcd_write_reg(0xC7);    //VCM control2 
	lcd_write_data(0XB7); 
	lcd_write_reg(0x36);    // Memory Access Control 
	lcd_write_data(0x48); 
	lcd_write_reg(0x3A);   
	lcd_write_data(0x55); 
	lcd_write_reg(0xB1);   
	lcd_write_data(0x00);   
	lcd_write_data(0x1A); 
	lcd_write_reg(0xB6);    // Display Function Control 
	lcd_write_data(0x0A); 
	lcd_write_data(0xA2); 
	lcd_write_reg(0xF2);    // 3Gamma Function Disable 
	lcd_write_data(0x00); 
	lcd_write_reg(0x26);    //Gamma curve selected 
	lcd_write_data(0x01); 
	lcd_write_reg(0xE0);    //Set Gamma 
	lcd_write_data(0x0F); 
	lcd_write_data(0x2A); 
	lcd_write_data(0x28); 
	lcd_write_data(0x08); 
	lcd_write_data(0x0E); 
	lcd_write_data(0x08); 
	lcd_write_data(0x54); 
	lcd_write_data(0XA9); 
	lcd_write_data(0x43); 
	lcd_write_data(0x0A); 
	lcd_write_data(0x0F); 
	lcd_write_data(0x00); 
	lcd_write_data(0x00); 
	lcd_write_data(0x00); 
	lcd_write_data(0x00); 		 
	lcd_write_reg(0XE1);    //Set Gamma 
	lcd_write_data(0x00); 
	lcd_write_data(0x15); 
	lcd_write_data(0x17); 
	lcd_write_data(0x07); 
	lcd_write_data(0x11); 
	lcd_write_data(0x06); 
	lcd_write_data(0x2B); 
	lcd_write_data(0x56); 
	lcd_write_data(0x3C); 
	lcd_write_data(0x05); 
	lcd_write_data(0x10); 
	lcd_write_data(0x0F); 
	lcd_write_data(0x3F); 
	lcd_write_data(0x3F); 
	lcd_write_data(0x0F); 
	lcd_write_reg(0x2B); 
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x01);
	lcd_write_data(0x3f);
	lcd_write_reg(0x2A); 
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0xef);	 
	lcd_write_reg(0x11); //Exit Sleep
	delay_ms(120);
	lcd_write_reg(0x29); //display on

	lcddev.id=0x9341;
	lcddev.dir=0;
	lcddev.width=240;
	lcddev.height=320;
	lcddev.wramcmd=0X2C;
	lcddev.setxcmd=0X2A;
	lcddev.setycmd=0X2B;
	
	lcd_scan_dir();
	LCD_LED = 1;
	
}

















