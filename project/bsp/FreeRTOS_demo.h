#ifndef __FREERTOS_DEMO_H_
#define __FREERTOS_DEMO_H_

/* freertos相关的头文件，必须的 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"  // 添加信号量头文件
extern void freertos_start(void);
/* 串口互斥锁 */
extern SemaphoreHandle_t uart_mutex;

#endif

