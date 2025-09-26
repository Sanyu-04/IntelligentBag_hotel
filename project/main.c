//-----------------------------------------------------------------------------
#include "ht32.h"
#include "GPIO.h"
#include "USART1.h"
#include "USART0.h"
#include "UART0.h"
#include "led.h"
#include "Delay.h"
#include "FreeRTOS_demo.h"
#include "WL-125.h"
#include "usartHMI.h"
#include "4G.h"
#include "ADS1115.h"

extern __ALIGN4 _USART1_STRUCT rxd_scomm1;
extern uint8_t id;
//-----------------------------------------------------------------------------
int main(void)
{
	
	GPIO_Configuration();
	ADS1115_GPIO_Init();
	USART1_Configuration();
	USART0_Configuration();
	UART1_Configuration();
	UART0_Configuration();
	USART1_SendByte('a');
	printf("¿ªÊ¼...\r\n");
//	HMI_send_number("course_table.b2.bco",65535);
	printf("week:%s\n",week);
	HMI_send_string("course_table.week1.txt",week);
	LED1_Turn_On();
	Delay_ms(1000);
	LED1_Turn_Off();
	
	
//	UART1_SendString("90");
	
	freertos_start();

	while (1)
	{
//		battery_adc();
		//test_4G();
//		USART0_SendString(week);
//		battery_adc();
//		printf("week:%s\n",week);
//		USART0_SendString("aa\n");
	}
}
