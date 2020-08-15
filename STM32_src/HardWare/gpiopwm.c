/*
TIM2&TIM3 PWM1 PC2
*/
#include <sys.h>

#include "gpiopwm.h"

char PWM_DJ;

void gpio_init()
{
	RCC->APB2ENR |= 1<<4;        //开GPIOC时钟
	GPIOC->CRL &= 0XFFFF00FF;	 //清除之前的设置
	GPIOC->CRL |= 0X00003300;	 //设置为输出
	GPIOC->ODR |= 3<<2;			 //上拉输出
}

//TIM2&TIM3 PWM1 PC2
void pwm1_init(u16 arr)
{
	gpio_init();					//开启GPIOC时钟
	RCC->APB1ENR |= 3; 				//开启TIM2&TIM3的时钟
	
	TIM2->ARR = arr;				//加载预装载定时器
	TIM3->ARR = PWM1_T;				//
	
	TIM2->PSC = 7199;				//加载分频器
	TIM3->PSC = 7199;				//
	
	TIM2->DIER |= 1<<0;				//开启中断
	TIM3->DIER |= 1<<0;				//
	
	TIM2->CR1 |= 0X01;				//开启定时器
	TIM3->CR1 |= 0X01;
	
	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//设置中断优先级和中断入口地址
	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//
}

//定时器2中断函数：输出低电平
void TIM2_IRQHandler()
{
	PWM1 = 0;
	TIM2->ARR = PWM1_T;
	TIM2->SR = ~(1<<0);	//清楚中断标志
}

//定时器3中断函数：输出高电平 20ms
void TIM3_IRQHandler()
{
	PWM1 = 1;
	TIM3->SR = ~(1<<0);	//清楚中断标志
}






