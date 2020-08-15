#include "exti.h"
#include "nrf24l01.h"
#include "lcd.h"
//#include "led.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ds18b20.h"

#include "gps.h"
//#include "usart1.h"
#include "key.h"
#include "timer.h"
#include "gpiopwm.h"
#include "tjq.h"
#include "usart2.h"
#include "timer8_pwm.h"
#include "dcf.h"

extern float DATA[3];
//extern u8 TEMP_BUF[USART1_MAX_RECV_LEN];

//外部中断1服务程序
void EXTI1_IRQHandler(void)
{
	nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);
	
	rx_command(rx_buf);
	
	nrf_writereg(NRF_WRITE_REG+STATUS,0x40);
	EXTI->PR = 1<<1;  //清除LINE0上的中断标志位  
}

//外部中断初始化程序
//初始化PA0,PC5,PA15为中断输入.
void rx_exti_init(void)
{
	RCC->APB2ENR |= 1<<2;
	GPIOA->CRL &= 0XFFFFFF0F;
	GPIOA->CRL |= 0X00000080;
	GPIOA->ODR |= 1<<1;
	
	Ex_NVIC_Config(GPIO_A,1,FTIR); 		//下降沿触发

	MY_NVIC_Init(2,2,EXTI1_IRQn,2);    	//抢占2，子优先级2，组2 
}

u8 tjq_sta = 0;
//各种命令
void rx_command(u8 *buf)
{
	u8 kkkk;
	u8 x = 80,y = 20;
	u8 i;
	u16 rxlen;
	nmea_msg gpsx; 											//GPS信息
		switch(buf[0])
		{
			case 0x00:
				
//				PWM_DJ = 40;
//				pwm1_init(PWM_DJ);
//				LCD_ShowNum(120,20,0,4,16);
				break;  
			case 0x01:
				PC8_PWM = 9;			//舵机
//				PWM_DJ = 12;
//				pwm1_init(PWM_DJ);
//				LCD_ShowNum(120,40,1,4,16);
				break;
			case 0x02:
				PC8_PWM = 15;
//				PWM_DJ = 15;
//				pwm1_init(PWM_DJ);
//				LCD_ShowNum(120,60,2,4,16);
				break;
			case 0x03:
				PC8_PWM = 21;
//				PWM_DJ = 18;
//				pwm1_init(PWM_DJ);
//				LCD_ShowNum(120,80,3,4,16);
				break;
			case 0x04:
//				LED1 = !LED1;
				Tuijinqi_Open();		//推进器
				PA8_PWM = 100;
//				LCD_ShowNum(120,120,6,4,16);
				break;
			case 0x05:
//				LED1 = !LED1;
				PA8_PWM = 120;
//				LCD_ShowNum(120,120,6,4,16);
				break;
			case 0x06:
//				LED1 = !LED1;
				PA8_PWM = 140;
//				LCD_ShowNum(120,140,7,4,16);
				break;
			case 0x07:
//				LED1 = !LED1;
				PA8_PWM = 160;
//				LCD_ShowNum(120,160,8,4,16);
				break;
			case 0x08:
//				LED1 = !LED1;
				PA8_PWM = 180;
//				LCD_ShowNum(120,180,9,4,16);
				break;
			case 0x09:									//命令5
//				LCD_ShowNum(120,100,5,4,16); //接受到0x05，显示4
				DATA[0] = DS18B20_Get_Temp()/10;//接受温度到data[0]
			
				FloatToChar();				 //数据转换
				for(i =0 ;i<5;i++)
				{
//					LCD_ShowNum(x,y,tx_buf[i],12,12);  
					y += 20; 
				}
				tx_mode();					 //转换成发送模式
				delay_ms(200);				 //延迟200毫秒
				tx_send();					 //发送数据
				rx_mode();					 //切换为接受模式
				break;
			case 0x10:
				Tuijinqi_Close();
				break;
			case 0x11:
				send_msg();
				break;
			case 0x12:
				DCF = !DCF;
			default:
				break;
		}
}












