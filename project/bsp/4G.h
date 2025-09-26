#ifndef __4G_H_
#define __4G_H_

#include "ht32.h"
#include "GPIO.h"
#include "USART1.h"
#include "USART0.h"
#include "led.h"
#include "Delay.h"
#include "FreeRTOS_demo.h"
#include "WL-125.h"
#include "usartHMI.h"
#include <jansson.h>
#include "UART0.h"
#include <stdint.h>
#include <string.h>

extern void key_infor(void);
extern void test_4G(void);
extern char week[20];//存放时间--星期几
extern u8 key_number;
extern void Find_key(void);
#endif



