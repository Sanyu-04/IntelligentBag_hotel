#include "LED.h"


/** 点亮LED
*/


void LED1_Turn_On(void)
{
	GPIO_ClearOutBits(OUT_PC14_GPIO_PORT,OUT_PC14_GPIO_PIN);
}

/** 熄灭LED
 */
void LED1_Turn_Off(void)
{
	GPIO_SetOutBits(OUT_PC14_GPIO_PORT,OUT_PC14_GPIO_PIN);
}


/** 翻转LED状态
 */
void LED1_Toggle(void)
{
//    HT_GPIO_TypeDef* HT_GPIOB = HT_GPIOB;
//    HT_GPIO_TypeDef* HT_GPIOC = HT_GPIOC;//已经定义过了

    // 读取PC14引脚的输出状态
    FlagStatus pc14Status = GPIO_ReadOutBit(HT_GPIOC, OUT_PC14_GPIO_PIN);
    if (pc14Status == SET)
    {
        LED1_Turn_On();
    }
    else
    {
        LED1_Turn_Off();
    }
}


