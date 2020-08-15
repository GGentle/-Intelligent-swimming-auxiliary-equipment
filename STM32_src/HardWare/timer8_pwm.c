/*
PC8/TIM8_CH3/
用于舵机的角度调节
*/

#include "sys.h"
#include "delay.h"

#include "timer8_pwm.h"

//开启GPIOC的时钟
void GPIOC_PWM_Init(void)
{
	RCC->APB2ENR|=1<<4;    //使能PORTC时钟	   	 
}

void TIM8_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<13; 	//TIM8时钟使能  
	GPIOC_PWM_Init();		//使能GPIOC
	GPIOC->CRH&=0XFFFFFFF0;	//PC8清除之前的设置
	GPIOC->CRH|=0X0000000B;	//复用功能输出 
	
	TIM8->ARR=arr;			//设定计数器自动重装值 
	TIM8->PSC=psc;			//预分频器设置
	
	//PC8
	TIM8->CCMR2|=6<<4;  	//CH3 PWM1模式		 
	TIM8->CCMR2|=1<<3; 		//CH3预装载使能	 

	//PC8
 	TIM8->CCER|=1<<8;   	//OC3 输出使能	 
	
	TIM8->BDTR|=1<<15;   	//MOE 主输出使能	   

	TIM8->CR1=0x0080;   	//ARPE使能 
	TIM8->CR1|=0x01;    	//使能定时器8										  
}