/*
先检查NRF和LCD屏
NRF设为接收模式

查询方式检查是否接收到数据 收到数据后判断命令类型并作出反应:
	1.改变舵机方向    3个方向
	2.改变气囊开关    开/关
	3.改变推进器开关  开/关
	
	若收到按键8的命令 则转换为发送模式并发送数据
	发送的数据有:
	1）温度
	2）位置

定时器5秒中断一次 
测试一次温度和位置，然后存储在特定位置上
手机端发出申请时就发出
接收GPS位置信息并显示在屏幕上

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
//nmea_msg gpsx; 											//GPS信息

int main()
{
	u8 sta;
	u8 i;
	u8 x = 80,y = 20;
	u16 rxlen;
	u16 lenx;
	int a;
	u8 key; 
	
	Stm32_Clock_Init(9);	//系统时钟设置
	delay_init(72);	   	 	//延时初始化 
	
	//温度传感器初始化
	DS18B20_Init();
	
	nrf_init();
	
	uart2_init(72,19200);
	
	Dcf_Init();				//电磁阀
	
	TIM1_PWM_Init(199,7200);//推进器初始状态为关闭
	Tuijinqi_Gpio_Init();	
	PA8_PWM = 80;
	Tuijinqi_Close();
	
	//舵机PWM
	TIM8_PWM_Init(199,7199);
	
	while(nrf_check());
	
	rx_mode();//接收模式
	while(1)
	{	
		rx_read();
		rx_command(rx_buf);                                //根据数据采取命令
		
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
//		LCD_ShowNum(60+40+8,150,temperature/10,2,16);	//显示正数部分	    
//		LCD_ShowNum(60+40+32,150,temperature%10,1,16);	//显示小数部分 
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