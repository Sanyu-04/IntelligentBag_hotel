//-----------------------------------------------------------------------------
#include "ht32.h"
#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

//-----------------------------------------------------------------------------
void NMI_Handler(void)
{
  while (1);
}

//-----------------------------------------------------------------------------
void HardFault_Handler(void)
{
  while (1);
  //NVIC_SystemReset(); //For the final version
}

//-----------------------------------------------------------------------------
//void SVC_Handler(void)
//{
//  while (1);
//}

////-----------------------------------------------------------------------------
//void PendSV_Handler(void)
//{
//  while (1);
//}

void SysTick_Handler(void)
{
//  tick_ct++;
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) 
    {
        xPortSysTickHandler();
    }
}

