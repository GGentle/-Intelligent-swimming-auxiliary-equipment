/*
�ȼ��NRF��LCD��
NRF��Ϊ����ģʽ

��ѯ��ʽ����Ƿ���յ����� �յ����ݺ��ж��������Ͳ�������Ӧ:
	1.�ı�������    3������
	2.�ı����ҿ���    ��/��
	3.�ı��ƽ�������  ��/��
	
	���յ�����8������ ��ת��Ϊ����ģʽ����������
	���͵�������:
	1���¶�
	2��λ��

��ʱ��5���ж�һ�� 
����һ���¶Ⱥ�λ�ã�Ȼ��洢���ض�λ����
�ֻ��˷�������ʱ�ͷ���
����GPSλ����Ϣ����ʾ����Ļ��

time:9.27

*/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "nrf24l01.h"
//#include "led.h"
#include "lcd.h"
#include "exti.h"
#include "ds18b20.h"

#include "gps.h"
//#include "usart1.h"
#include "key.h"
#include "timer.h"
#include "gpiopwm.h"
#include "usart2.h" //sim900a
#include "dcf.h"
#include "tjq.h"
#include "timer8_pwm.h"

extern float DATA[3];
//u8 TEMP_BUF[USART1_MAX_RECV_LEN];
//nmea_msg gpsx; 											//GPS��Ϣ

int main()
{
	u8 sta;
	u8 i;
	u8 x = 80,y = 20;
	u16 rxlen;
	u16 lenx;
	int a;
	u8 key; 
	
	Stm32_Clock_Init(9);	//ϵͳʱ������
	delay_init(72);	   	 	//��ʱ��ʼ�� 
	
	//�¶ȴ�������ʼ��
	DS18B20_Init();
	
	nrf_init();
	
	uart2_init(72,19200);
	
	Dcf_Init();				//��ŷ�
	
	TIM1_PWM_Init(199,7200);//�ƽ�����ʼ״̬Ϊ�ر�
	Tuijinqi_Gpio_Init();	
	PA8_PWM = 80;
	Tuijinqi_Close();
	
	//���PWM
	TIM8_PWM_Init(199,7199);
	
	while(nrf_check());
	
	rx_mode();//����ģʽ
	while(1)
	{	
		rx_read();
		rx_command(rx_buf);                                //�������ݲ�ȡ����
		
		spi1_rw(FLUSH_RX);
		rx_buf[0] = 0xff;
	}

	return 0;
}

//		tx_mode();
//		sta = nrf_readdata(NRF_READ_REG+STATUS);
//		if(sta&0x20)
//		{
//			LED0 = ~LED0;
//			delay_ms(5000);
//		}

//	rx_mode();
//	while(1)
//	{
//		nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);
//		if(rx_buf[0] == 0x01)
//		{
//			LED0 = 1;
//		}
//	}


//	while(1)
//	{
//		temperature=DS18B20_Get_Temp();
//		LCD_ShowNum(60+40+8,150,temperature/10,2,16);	//��ʾ��������	    
//		LCD_ShowNum(60+40+32,150,temperature%10,1,16);	//��ʾС������ 
//		delay_ms(500);
//	}

//	while(1)
//	{
//		DATA[0] = DS18B20_Get_Temp()/10;
//		LCD_ShowNum(x,200,DATA[0],12,12); 
//		FloatToChar();
//		for(i =0 ;i<5;i++)
//		{
//			LCD_ShowNum(x,y,tx_buf[i],12,12);  
//			y += 20; 
//		}
//		y = 20;
//		delay_ms(1000);
//	}
	
//	tx_mode();	
//	while(1)
//	{
//		tx_send();
//	}