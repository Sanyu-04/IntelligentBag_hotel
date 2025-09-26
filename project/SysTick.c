//-----------------------------------------------------------------------------
#include "SysTick.h"

//-----------------------------------------------------------------------------
#define TICK_TIMER_BASE            ((long long)SystemCoreClock * 1/1000)

//-----------------------------------------------------------------------------
vu32 tick_ct;

//-----------------------------------------------------------------------------
void SysTick_Configuration(void)
{
  SYSTICK_ClockSourceConfig(SYSTICK_SRC_FCLK);
  SYSTICK_SetReloadValue(TICK_TIMER_BASE);
  SYSTICK_IntConfig(ENABLE);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
}

//-----------------------------------------------------------------------------
//void SysTick_Handler(void)
//{
////  tick_ct++;
//	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) 
//    {
//        xPortSysTickHandler();
//    }
//}

//-----------------------------------------------------------------------------


