#ifndef __FREERTOS_DEMO_H_
#define __FREERTOS_DEMO_H_

/* freertos��ص�ͷ�ļ�������� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"  // ����ź���ͷ�ļ�
extern void freertos_start(void);
/* ���ڻ����� */
extern SemaphoreHandle_t uart_mutex;

#endif

