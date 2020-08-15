#include "delay.h"
#include "USART1.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//���ڷ��ͻ����� 	
__align(8) u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�
#ifdef USART1_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//���ջ���,���USART1_MAX_RECV_LEN���ֽ�.


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART1_RX_STA=0;   	 
void USART1_IRQHandler(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR; 			 
		if(USART1_RX_STA<USART1_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM4->CNT=0;         					//���������
			if(USART1_RX_STA==0)TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			USART1_RX_BUF[USART1_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART1_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  											 
}   
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART1_Init(u32 pclk1,u32 bound)
{  	 		  
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬���� 
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=(pclk1*1000000)/(bound);// ����������	 
	USART1->CR1|=0X200C;  	//1λֹͣ,��У��λ.	
	
	USART1->CR3=1<<7;   	//ʹ�ܴ���1��DMA����
	UART_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUSART1_TX_BUF 
#ifdef USART1_RX_EN		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    	//PE�ж�ʹ��
	USART1->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(2,3,USART1_IRQn,2);//��2��������ȼ� 
	TIM4_Init(99,7199);		//10ms�ж�
	USART1_RX_STA=0;		//����
	TIM4_Set(0);			//�رն�ʱ��4
#endif										  	
}
//����2,printf ����
//ȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�
void u1_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA1_Channel4->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel4,strlen((const char*)USART1_TX_BUF)); 	//ͨ��dma���ͳ�ȥ
}
//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//�Ǹ����ж�
	{	 			   
		USART1_RX_STA|=1<<15;	//��ǽ������
		TIM4->SR&=~(1<<0);		//����жϱ�־λ		   
		TIM4_Set(0);			//�ر�TIM4  
	}	    
}
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
    	TIM4->CNT=0;         //���������
		TIM4->CR1|=1<<0;     //ʹ�ܶ�ʱ��4
	}else TIM4->CR1&=~(1<<0);//�رն�ʱ��4	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ   
	TIM4->PSC=psc;  	//Ԥ��Ƶ��
 	TIM4->DIER|=1<<0;   //��������ж�				
 	TIM4->CR1|=0x01;  	//ʹ�ܶ�ʱ��4	  	   
   	MY_NVIC_Init(1,3,TIM4_IRQn,2);//��ռ2�������ȼ�3����2	��2�����ȼ����								 
}
#endif		 
///////////////////////////////////////USART1 DMA�������ò���//////////////////////////////////	   		    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
 	RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	delay_us(5);
	DMA_CHx->CPAR=cpar; 		//DMA1 �����ַ 
	DMA_CHx->CMAR=cmar; 		//DMA1,�洢����ַ	 
	DMA_CHx->CCR=0X00000000;	//��λ
	DMA_CHx->CCR|=1<<4;  		//�Ӵ洢����
	DMA_CHx->CCR|=0<<5;  		//��ͨģʽ
	DMA_CHx->CCR|=0<<6;  		//�����ַ������ģʽ
	DMA_CHx->CCR|=1<<7;  		//�洢������ģʽ
	DMA_CHx->CCR|=0<<8;  		//�������ݿ��Ϊ8λ
	DMA_CHx->CCR|=0<<10; 		//�洢�����ݿ��8λ
	DMA_CHx->CCR|=1<<12; 		//�е����ȼ�
	DMA_CHx->CCR|=0<<14; 		//�Ǵ洢�����洢��ģʽ		  	
} 
//����һ��DMA����
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
	DMA_CHx->CNDTR=len;          //DMA1,���������� 
	DMA_CHx->CCR|=1<<0;          //����DMA����
}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 





















