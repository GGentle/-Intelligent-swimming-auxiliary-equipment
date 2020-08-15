/*
PA2
*/
#ifndef TJQ_H
#define TJQ_H

#include "sys.h"

#define PWM_EN PDout(2)

void Tuijinqi_Gpio_Init();
void Tuijinqi_Open();
void Tuijinqi_Close();
#endif