#ifndef GPIOPWM_H
#define GPIOPWM_H

#include <sys.h>

extern char PWM_DJ;

#define PWM1 PCout(2)
#define PWM1_T 200		//周期 = PWM1_T/10 ms

//参数：高电平的时间 = arr/10 ms
void pwm1_init(u16 arr);

/*
	PWM_DJ = 40;
	pwm1_init(PWM_DJ);			//TIM2&TIM3 PWM1 PC2
	
	TIM1_PWM_Init(199,7200);//PA8/TIM1_CH1
	PA8_PWM = 40;
*/

#endif