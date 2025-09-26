#ifndef __LED_H
#define __LED_H

#include "gpio.h"


#define led2 OUT_PB5_PIN
#define led1 OUT_PC14_GPIO_PIN

void LED1_Turn_On(void);
void LED2_Turn_On(void);
void LED1_Turn_Off(void);
void LED2_Turn_Off(void);

void LED1_Toggle(void);
void LED2_Toggle(void);


#endif
