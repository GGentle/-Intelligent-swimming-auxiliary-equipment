#ifndef TIMER8_PWM_H
#define TIMER8_PWM_H

#include "sys.h"

#define PC8_PWM TIM8->CCR3

void TIM8_PWM_Init(u16 arr,u16 psc);

/*
	TIM8_PWM_Init(199,7200);//PC8/TIM8_CH3
	PC8_PWM = 40;
*/

#endif