#ifndef __WL_125_H
#define __WL_125_H

#include "FreeRTOS_demo.h"

#include "USART0.h"
#include "USART1.h"
#include "UART1.h"
#include <stdio.h>  //用于打印课表信息
#include "string.h"
#include "usartHMI.h"
#include "4G.h"


//#define Reset_WL_Pin HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin,GPIO_PIN_RESET)	//定义模块复位引脚

#define MAX_BOOK_NAME_LENGTH 50
#define MAX_COURSE_NUM 7
extern void InputNewCourse(void);			//录入新课表
extern void printf_course_table(void);		//打印已录入课表
extern void print_today_course_table(void);	//打印当天课表
extern void CourseTableDetection(void);		//课表检测
// 新函数：打印缺失课本数组
void PrintMissingBooks(void);

extern uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH];
extern uint8_t detected_count;

extern void Reset_WL(void);			//复位模块

extern u8 is_inputting;//控制是否录入标志位
extern u8 course_count;  		//当前课表中的课本数量

#endif

