/*
所用硬件资源
PA8/TIM1_CH1
用于推进器速度调节
*/
#include "timer.h"  
u8 pwm_dj[3] = {50,70,90};
u8 pwm_tjq[5] = {20,40,60,80,100};

//开启GPIOA的时钟
void GPIOA_PWM_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
}

void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<11; 	//TIM1时钟使能  
	GPIOA_PWM_Init();		//使能GPIOC
	GPIOA->CRH&=0XFFFFFFF0;	//PA8清除之前的设置
	GPIOA->CRH|=0X0000000B;	//复用功能输出 
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器设置
	
	//PC9
	TIM1->CCMR1|=6<<4;  	//CH1 PWM1模式		 
	TIM1->CCMR1|=1<<3; 		//CH1预装载使能	 

	//PC9
 	TIM1->CCER|=1<<0;   	//OC1 输出使能	 
	
	TIM1->BDTR|=1<<15;   	//MOE 主输出使能	   

	TIM1->CR1=0x0080;   	//ARPE使能 
	TIM1->CR1|=0x01;    	//使能定时器1 										  
}















