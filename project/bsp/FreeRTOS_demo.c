#include "FreeRTOS_demo.h"
/* freertos��ص�ͷ�ļ�������� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"  // ����ź���ͷ�ļ�
/* ��Ҫ�õ�������ͷ�ļ� */
#include "LED.h"
#include "WL-125.h"
#include "ADS1115.h"

/* ������������� */
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void *pvParameters);//void *������ָ��

/*����1������*/
#define TASK1_STACK 128
#define TASK1_PRIORITY 3
TaskHandle_t task1_handle;
void task1(void *pvParameters);

/*����2������*/
#define TASK2_STACK 1024
#define TASK2_PRIORITY 4
TaskHandle_t task2_handle;
void task2(void *pvParameters);

/*����3������*/
#define TASK3_STACK 256
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
void task3(void *pvParameters);

///*����4������*/
//#define TASK4_STACK 512
//#define TASK4_PRIORITY 2
//TaskHandle_t task4_handle;
//void task4(void *pvParameters);

///*����5������*/
//#define TASK5_STACK 512
//#define TASK5_PRIORITY 3
//TaskHandle_t task5_handle;
//void task5(void *pvParameters);

/* ���ڻ����� */
SemaphoreHandle_t uart_mutex;


//�����ź����ģ���ʱ����Ҫ�õ�
#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook(TaskHandle_t xTask,char * pcTaskName )
{
	portDISABLE_INTERRUPTS();
	printf("\nStack overflow!\n");
	while(1);
}
#endif



/**
 * @description: ����FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{	
    /* 1.����һ���������� */
    xTaskCreate((TaskFunction_t)start_task,               // �������ĵ�ַ
                (char *)"start_task",                     // �������ַ���
                (configSTACK_DEPTH_TYPE)START_TASK_STACK, // ����ջ��С��Ĭ����С128����λ4�ֽ�
                (void *)NULL,                             // ���ݸ�����Ĳ���
                (UBaseType_t)START_TASK_PRIORITY,         // ��������ȼ�
                (TaskHandle_t *)&start_task_handle);      // �������ĵ�ַ

    /* 2.����������:���Զ������������� */
    vTaskStartScheduler();
}

/**
 * @description: ��������������������Task
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    /* �����ٽ���:�����ٽ�����Ĵ��벻�ᱻ��� */
    taskENTER_CRITICAL();

    /* ����3������ */
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
    /* ��������ֻ��Ҫִ��һ�μ��ɣ������ɾ���Լ� */
    vTaskDelete(NULL);

    /* �˳��ٽ��� */
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
		printf("\ntask1����.....................\r\n");
		//¼��α�
		InputNewCourse();//�α�¼��
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
		printf("\ntask2����.....................\r\n");
		
        test_4G();
		HMI_send_string("course_table.week1.txt",week);
		
		vTaskDelay(500);
	}
}

/**
 * @description: ������:
 * @param {void} *pvParameters
 * @return {*}
 */
void task3(void *pvParameters)
{
//	HMI_send_number("course_table.b2.bco",65535);
//	HMI_send_number("course_table.b2.bco2",65535);
	while(1)
	{
		printf("\ntask3����.....................\r\n");
		battery_adc();
		while(detect_flag)
		{
			//���α�
			CourseTableDetection();
		}
		vTaskDelay(500);
		
	}
}


/**
 * @description: ������:
 * @param {void} *pvParameters
 * @return {*}
 */
void task4(void *pvParameters)
{
	while(1)
	{
		printf("task4����...\r\n");
		battery_adc();
		vTaskDelay(500);
	}
}

/**
 * @description: ������:�๦�ܰ���
 * @param {void} *pvParameters
 * @return {*}
 */
void task5(void *pvParameters)
{
	while(1)
	{
		printf("task5����...\r\n");
		Find_key();
		vTaskDelay(500);
	}
}
