#include "usart2.h"	
#include "delay.h"
#include "sys.h"

//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif 
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//等待上一次串口数据发送完成  
	USART2->DR = (u8) ch;      	//写DR,串口2将发送数据
	return ch;
}
#endif 
//end

#if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART2_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
//u16 USART2_RX_STA=0;       //接收状态标记	  

u8  USART2_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
u16 USART2_RX_STA;         		//接收状态标记	

void USART2_IRQHandler(void)
{
	u8 res;	
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))	//接收到数据
	{	 
		res=USART2->DR; 
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif										 
//初始化IO 串口2
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB1ENR|=1<<17;  //使能串口时钟 
	GPIOA->CRL&=0XFFFF00FF;//IO状态设置
	GPIOA->CRL|=0X00008B00;//IO状态设置 
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //1位停止,无校验位.
#if EN_USART2_RX		  //如果使能了接收
	//使能接收中断 
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//组2，最低优先级 
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
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg2);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg2[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg3);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg3[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_msg4);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_msg4[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);			
	USART2->DR=send_msg5;
	while((USART2->SR&0X40)==0);//等待发送结束	
}

u8 send_tcp1[] = "AT+CGCLASS=\"B\"\r\n";
u8 send_tcp2[] = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
u8 send_tcp3[] = "AT+CGATT=1\r\n";
u8 send_tcp4[] = "AT+CIPCSGP=1,\"CMNET\"\r\n";
u8 send_tcp5[] = "AT+CLPORT=\"TCP\",\"2000\"\r\n";
u8 send_tcp6[] = "AT+CIPSTART=\"TCP\",\"103.44.145.247\",\"18271\"\r\n";
u8 send_tcp7[] = "AT+CIPSEND\r\n";

u8 send_tcp8[] = "温度：28 经度111.44，纬度121.88";
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
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp2);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp2[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp3);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp3[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp4);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp4[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp5);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp5[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp6);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp6[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp7);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp7[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);
			
	len = sizeof(send_tcp8);
	for(t=0;t<len;t++)
	{
		USART2->DR=send_tcp8[t];
		while((USART2->SR&0X40)==0);//等待发送结束
	}
	delay_ms(TIME);

	USART2->DR=send_tcp13;
	while((USART2->SR&0X40)==0);//等待发送结束	
}

















