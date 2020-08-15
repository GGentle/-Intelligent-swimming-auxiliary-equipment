#include "sys.h"

#include "dcf.h"

void Dcf_Init()
{
	RCC->APB2ENR |= 1<<4;
	GPIOC->CRL &= 0XFF0FFFFF;
	GPIOC->CRL |= 0X00300000;
}