#ifndef __WL_125_H
#define __WL_125_H

#include "FreeRTOS_demo.h"

#include "USART0.h"
#include "USART1.h"
#include "UART1.h"
#include <stdio.h>  //���ڴ�ӡ�α���Ϣ
#include "string.h"
#include "usartHMI.h"
#include "4G.h"


//#define Reset_WL_Pin HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin,GPIO_PIN_RESET)	//����ģ�鸴λ����

#define MAX_BOOK_NAME_LENGTH 50
#define MAX_COURSE_NUM 7
extern void InputNewCourse(void);			//¼���¿α�
extern void printf_course_table(void);		//��ӡ��¼��α�
extern void print_today_course_table(void);	//��ӡ����α�
extern void CourseTableDetection(void);		//�α���
// �º�������ӡȱʧ�α�����
void PrintMissingBooks(void);

extern uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH];
extern uint8_t detected_count;

extern void Reset_WL(void);			//��λģ��

extern u8 is_inputting;//�����Ƿ�¼���־λ
extern u8 course_count;  		//��ǰ�α��еĿα�����

#endif

