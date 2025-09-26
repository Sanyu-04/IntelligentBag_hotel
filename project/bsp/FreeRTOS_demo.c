#include "FreeRTOS_demo.h"
/* freertos相关的头文件，必须的 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"  // 添加信号量头文件
/* 需要用到的其他头文件 */
#include "LED.h"
#include "WL-125.h"
#include "ADS1115.h"

/* 启动任务的配置 */
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void *pvParameters);//void *无类型指针

/*任务1的配置*/
#define TASK1_STACK 128
#define TASK1_PRIORITY 3
TaskHandle_t task1_handle;
void task1(void *pvParameters);

/*任务2的配置*/
#define TASK2_STACK 1024
#define TASK2_PRIORITY 4
TaskHandle_t task2_handle;
void task2(void *pvParameters);

/*任务3的配置*/
#define TASK3_STACK 256
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
void task3(void *pvParameters);

///*任务4的配置*/
//#define TASK4_STACK 512
//#define TASK4_PRIORITY 2
//TaskHandle_t task4_handle;
//void task4(void *pvParameters);

///*任务5的配置*/
//#define TASK5_STACK 512
//#define TASK5_PRIORITY 3
//TaskHandle_t task5_handle;
//void task5(void *pvParameters);

/* 串口互斥锁 */
SemaphoreHandle_t uart_mutex;


//关于信号量的，暂时不需要用到
#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook(TaskHandle_t xTask,char * pcTaskName )
{
	portDISABLE_INTERRUPTS();
	printf("\nStack overflow!\n");
	while(1);
}
#endif



/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{	
    /* 1.创建一个启动任务 */
    xTaskCreate((TaskFunction_t)start_task,               // 任务函数的地址
                (char *)"start_task",                     // 任务名字符串
                (configSTACK_DEPTH_TYPE)START_TASK_STACK, // 任务栈大小，默认最小128，单位4字节
                (void *)NULL,                             // 传递给任务的参数
                (UBaseType_t)START_TASK_PRIORITY,         // 任务的优先级
                (TaskHandle_t *)&start_task_handle);      // 任务句柄的地址

    /* 2.启动调度器:会自动创建空闲任务 */
    vTaskStartScheduler();
}

/**
 * @description: 启动任务：用来创建其他Task
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    /* 进入临界区:保护临界区里的代码不会被打断 */
    taskENTER_CRITICAL();

    /* 创建3个任务 */
    xTaskCreate((TaskFunction_t)task1,
                (char *)"task1",
                (configSTACK_DEPTH_TYPE)TASK1_STACK,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIORITY,
                (TaskHandle_t *)&task1_handle);
    xTaskCreate((TaskFunction_t)task2,
                (char *)"task2",
                (configSTACK_DEPTH_TYPE)TASK2_STACK,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIORITY,
                (TaskHandle_t *)&task2_handle);
    xTaskCreate((TaskFunction_t)task3,
                (char *)"task3",
                (configSTACK_DEPTH_TYPE)TASK3_STACK,
                (void *)NULL,
                (UBaseType_t)TASK3_PRIORITY,
                (TaskHandle_t *)&task3_handle);
//	xTaskCreate((TaskFunction_t)task4,
//                (char *)"task4",
//                (configSTACK_DEPTH_TYPE)TASK4_STACK,
//                (void *)NULL,
//                (UBaseType_t)TASK4_PRIORITY,
//                (TaskHandle_t *)&task4_handle);
//	xTaskCreate((TaskFunction_t)task5,
//                (char *)"task5",
//                (configSTACK_DEPTH_TYPE)TASK5_STACK,
//                (void *)NULL,
//                (UBaseType_t)TASK5_PRIORITY,
//                (TaskHandle_t *)&task5_handle);
    /* 启动任务只需要执行一次即可，用完就删除自己 */
    vTaskDelete(NULL);

    /* 退出临界区 */
	taskEXIT_CRITICAL();
}


/**
 * @description: 
 * @param {void} *pvParameters
 * @return {*}
 */
void task1(void *pvParameters)
{
	while(1)
	{
		printf("\ntask1运行.....................\r\n");
		//录入课表
		InputNewCourse();//课本录入
		vTaskDelay(700);
	}
}

/**
 * @description: 
 * @param {void} *pvParameters
 * @return {*}
 */
void task2(void *pvParameters)
{
	while(1)
	{
		printf("\ntask2运行.....................\r\n");
		
        test_4G();
		HMI_send_string("course_table.week1.txt",week);
		
		vTaskDelay(500);
	}
}

/**
 * @description: 任务三:
 * @param {void} *pvParameters
 * @return {*}
 */
void task3(void *pvParameters)
{
//	HMI_send_number("course_table.b2.bco",65535);
//	HMI_send_number("course_table.b2.bco2",65535);
	while(1)
	{
		printf("\ntask3运行.....................\r\n");
		battery_adc();
		while(detect_flag)
		{
			//检测课表
			CourseTableDetection();
		}
		vTaskDelay(500);
		
	}
}


/**
 * @description: 任务四:
 * @param {void} *pvParameters
 * @return {*}
 */
void task4(void *pvParameters)
{
	while(1)
	{
		printf("task4运行...\r\n");
		battery_adc();
		vTaskDelay(500);
	}
}

/**
 * @description: 任务五:多功能按键
 * @param {void} *pvParameters
 * @return {*}
 */
void task5(void *pvParameters)
{
	while(1)
	{
		printf("task5运行...\r\n");
		Find_key();
		vTaskDelay(500);
	}
}
