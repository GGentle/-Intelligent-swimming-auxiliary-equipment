#include "usart2.h"	
#include "delay.h"
#include "sys.h"

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif 
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART2->DR = (u8) ch;      	//дDR,����2����������
	return ch;
}
#endif 
//end

#if EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART2_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART2_RX_STA=0;       //����״̬���	  

u8  USART2_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART2_RX_STA;         		//����״̬���	

void USART2_IRQHandler(void)
{
	u8 res;	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))	//���յ�����
	{	 
		res=USART2->DR; 
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif										 
//��ʼ��IO ����2
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL&=0XFFFF00FF;//IO״̬����
	GPIOA->CRL|=0X00008B00;//IO״̬���� 
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART2_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
}

u8 send_msg1[] = "AT+CSCS=\"GSM\"\r\n";
u8 send_msg2[] = "AT+CMGF=1\r\n";
u8 send_msg3[] = "AT+CMGS=\"15627864687\"\r\n";
u8 send_msg4[] = "ATK-SIM900A MSG SEND TEST\r\n";
u8 send_msg5 = 0x1a;

void send_msg()
{
	u8 len;
	u8 t;
	
	len = sizeof(send_msg1);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg1[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg2);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg2[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg3);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg3[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg4);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg4[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);			
	USART2->DR=send_msg5;
	while((USART2->SR&0X40)==0);//�ȴ����ͽ���	
}

u8 send_tcp1[] = "AT+CGCLASS=\"B\"\r\n";
u8 send_tcp2[] = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
u8 send_tcp3[] = "AT+CGATT=1\r\n";
u8 send_tcp4[] = "AT+CIPCSGP=1,\"CMNET\"\r\n";
u8 send_tcp5[] = "AT+CLPORT=\"TCP\",\"2000\"\r\n";
u8 send_tcp6[] = "AT+CIPSTART=\"TCP\",\"103.44.145.247\",\"18271\"\r\n";
u8 send_tcp7[] = "AT+CIPSEND\r\n";

u8 send_tcp8[] = "�¶ȣ�28 ����111.44��γ��121.88";
u8 send_tcp9[] = "111";
u8 send_tcp10[] = "48";
u8 send_tcp11[] = "83";
u8 send_tcp12[] = "45";
//u8 send_tcp8[5] = {28,111,22,89,44};
u8 send_tcp13 = 0x1a;

void send_gps_tmp()
{
	u8 len;
	u8 t;
	
	len = sizeof(send_tcp1);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp1[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp2);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp2[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp3);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp3[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp4);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp4[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp5);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp5[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp6);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp6[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp7);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp7[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp8);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp8[t];
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
	delay_ms(TIME);

	USART2->DR=send_tcp13;
	while((USART2->SR&0X40)==0);//�ȴ����ͽ���	
}

















