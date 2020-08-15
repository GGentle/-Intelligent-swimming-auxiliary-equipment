#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//TIM1_CH1，PWM输出设置相关内容 
////////////////////////////////////////////////////////////////////////////////// 	  
//通过改变TIM1->CCR1的值来改变占空比，从而控制LED0的亮度
    
extern u8 pwm_dj[3];
extern u8 pwm_tjq[5];

#define PA8_PWM TIM1->CCR1

void TIM1_PWM_Init(u16 arr,u16 psc);

/*
	PWM_DJ = 40;
	pwm1_init(PWM_DJ);			//TIM2&TIM3 PWM1 PC2
	
	TIM1_PWM_Init(199,7200);//PA8/TIM1_CH1
	PA8_PWM = 40;
*/

#endif























