#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
#include "stdio.h"	  
 
 #define TIME 200
 
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����
	  	
extern u8  USART2_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	

void uart2_init(u32 pclk2,u32 bound);	//uart2_init(72,19200);
void send_msg();
void send_gps_tmp();

#endif	   
















