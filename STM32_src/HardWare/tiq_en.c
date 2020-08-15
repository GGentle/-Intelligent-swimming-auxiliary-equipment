//用于推进器的开关
#include "sys.h"
#include "tjq.h"

void Tuijinqi_Gpio_Init()
{
	RCC->APB2ENR |= 1<<5;
	GPIOD->CRL &= 0XFFFFF0FF;
	GPIOD->CRL |= 0X00000300;
//	GPIOA->ODR |= ~(1<<14);
}

void Tuijinqi_Open()
{
	PWM_EN = 1;
}

void Tuijinqi_Close()
{
	PWM_EN = 0;
}