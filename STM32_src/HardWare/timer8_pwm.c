/*
PC8/TIM8_CH3/
���ڶ���ĽǶȵ���
*/

#include "sys.h"
#include "delay.h"

#include "timer8_pwm.h"

//����GPIOC��ʱ��
void GPIOC_PWM_Init(void)
{
	RCC->APB2ENR|=1<<4;    //ʹ��PORTCʱ��	   	 
}

void TIM8_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<13; 	//TIM8ʱ��ʹ��  
	GPIOC_PWM_Init();		//ʹ��GPIOC
	GPIOC->CRH&=0XFFFFFFF0;	//PC8���֮ǰ������
	GPIOC->CRH|=0X0000000B;	//���ù������ 
	
	TIM8->ARR=arr;			//�趨�������Զ���װֵ 
	TIM8->PSC=psc;			//Ԥ��Ƶ������
	
	//PC8
	TIM8->CCMR2|=6<<4;  	//CH3 PWM1ģʽ		 
	TIM8->CCMR2|=1<<3; 		//CH3Ԥװ��ʹ��	 

	//PC8
 	TIM8->CCER|=1<<8;   	//OC3 ���ʹ��	 
	
	TIM8->BDTR|=1<<15;   	//MOE �����ʹ��	   

	TIM8->CR1=0x0080;   	//ARPEʹ�� 
	TIM8->CR1|=0x01;    	//ʹ�ܶ�ʱ��8										  
}