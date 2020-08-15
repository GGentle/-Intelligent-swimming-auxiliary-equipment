#ifndef LCD_H
#define LCD_H

#include "sys.h"

#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //Ƭѡ�˿�  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //����/���� 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //д����			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //������			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //Ƭѡ�˿�  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //����/����		PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //д����			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //������			PC6  
//BL PC10
#define	LCD_LED PCout(10) 				//LCD����    	PC10 

//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 

//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

void lcd_write_data(u16 data);//д����
u16 lcd_read_data();//������
void lcd_write_reg(u16 data);//д����

void lcd_write(u16 cmd,u16 data);//д����
u16 lcd_read(u16 cmd);//������

u8 lcd_check();//�����ʾ��ID
void lcd_init();//��ʼ������
void write_ram_prepare();//׼��дGRAM
void write_ram(u16 data);//дRAM
void lcd_scan_dir();//�洢����д�������
void lcd_setcursor(u16 x,u16 y);//������ú���
void lcd_setcorlor(u16 color);//������Ļ��ɫ
void draw_line(u16 x1,u16 y1,u16 x2,u16 y2);//����

//��ʾ���ֺ�Ӣ��
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//���ٻ���
u32 LCD_Pow(u8 m,u8 n);

void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//��ʾһ������
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ���,12/16����
//void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����


//������ɫ
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
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ

#endif