#ifndef __ADS1115_H
#define __ADS1115_H


#include "ht32.h"

#include "stdio.h"
#include "GPIO.h"
#include "delay.h"

void ADS1115_GPIO_Init(void);
unsigned char WriteADS1115(unsigned char add,unsigned char dat_H,unsigned char dat_L);
float ReadADS1115(unsigned char add);
void battery_adc(void);

#endif


