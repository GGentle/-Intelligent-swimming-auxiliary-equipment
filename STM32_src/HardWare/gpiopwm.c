/*
TIM2&TIM3 PWM1 PC2
*/
#include <sys.h>

#include "gpiopwm.h"

char PWM_DJ;

void gpio_init()
{
	RCC->APB2ENR |= 1<<4;        //��GPIOCʱ��
	GPIOC->CRL &= 0XFFFF00FF;	 //���֮ǰ������
	GPIOC->CRL |= 0X00003300;	 //����Ϊ���
	GPIOC->ODR |= 3<<2;			 //�������
}

//TIM2&TIM3 PWM1 PC2
void pwm1_init(u16 arr)
{
	gpio_init();					//����GPIOCʱ��
	RCC->APB1ENR |= 3; 				//����TIM2&TIM3��ʱ��
	
	TIM2->ARR = arr;				//����Ԥװ�ض�ʱ��
	TIM3->ARR = PWM1_T;				//
	
	TIM2->PSC = 7199;				//���ط�Ƶ��
	TIM3->PSC = 7199;				//
	
	TIM2->DIER |= 1<<0;				//�����ж�
	TIM3->DIER |= 1<<0;				//
	
	TIM2->CR1 |= 0X01;				//������ʱ��
	TIM3->CR1 |= 0X01;
	
	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//�����ж����ȼ����ж���ڵ�ַ
	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//
}

//��ʱ��2�жϺ���������͵�ƽ
void TIM2_IRQHandler()
{
	PWM1 = 0;
	TIM2->ARR = PWM1_T;
	TIM2->SR = ~(1<<0);	//����жϱ�־
}

//��ʱ��3�жϺ���������ߵ�ƽ 20ms
void TIM3_IRQHandler()
{
	PWM1 = 1;
	TIM3->SR = ~(1<<0);	//����жϱ�־
}






