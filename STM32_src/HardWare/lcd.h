#ifndef LCD_H
#define LCD_H

#include "sys.h"

#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //片选端口  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //数据/命令 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //写数据			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //读数据			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //片选端口  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //数据/命令		PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //写数据			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //读数据			PC6  
//BL PC10
#define	LCD_LED PCout(10) 				//LCD背光    	PC10 

//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16 setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令	 
}_lcd_dev; 

//LCD的画笔颜色和背景色	   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色

void lcd_write_data(u16 data);//写数据
u16 lcd_read_data();//读数据
void lcd_write_reg(u16 data);//写命令

void lcd_write(u16 cmd,u16 data);//写操作
u16 lcd_read(u16 cmd);//读操作

u8 lcd_check();//检查显示屏ID
void lcd_init();//初始化函数
void write_ram_prepare();//准备写GRAM
void write_ram(u16 data);//写RAM
void lcd_scan_dir();//存储器读写方向控制
void lcd_setcursor(u16 x,u16 y);//光标设置函数
void lcd_setcorlor(u16 color);//设置屏幕颜色
void draw_line(u16 x1,u16 y1,u16 x2,u16 y2);//画线

//显示数字和英文
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//快速画点
u32 LCD_Pow(u8 m,u8 n);

void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//显示一个字符
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//显示一个数字
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//显示一个字符串,12/16字体
//void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//显示 数字


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

#endif