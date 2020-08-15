/*
����Ӳ����Դ
PA8/TIM1_CH1
�����ƽ����ٶȵ���
*/
#include "timer.h"  
u8 pwm_dj[3] = {50,70,90};
u8 pwm_tjq[5] = {20,40,60,80,100};

//����GPIOA��ʱ��
void GPIOA_PWM_Init(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
}

void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<11; 	//TIM1ʱ��ʹ��  
	GPIOA_PWM_Init();		//ʹ��GPIOC
	GPIOA->CRH&=0XFFFFFFF0;	//PA8���֮ǰ������
	GPIOA->CRH|=0X0000000B;	//���ù������ 
	
	TIM1->ARR=arr;			//�趨�������Զ���װֵ 
	TIM1->PSC=psc;			//Ԥ��Ƶ������
	
	//PC9
	TIM1->CCMR1|=6<<4;  	//CH1 PWM1ģʽ		 
	TIM1->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	 

	//PC9
 	TIM1->CCER|=1<<0;   	//OC1 ���ʹ��	 
	
	TIM1->BDTR|=1<<15;   	//MOE �����ʹ��	   

	TIM1->CR1=0x0080;   	//ARPEʹ�� 
	TIM1->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 										  
}















