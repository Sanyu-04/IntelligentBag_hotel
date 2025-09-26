#include "LED.h"


/** ����LED
*/


void LED1_Turn_On(void)
{
	GPIO_ClearOutBits(OUT_PC14_GPIO_PORT,OUT_PC14_GPIO_PIN);
}

/** Ϩ��LED
 */
void LED1_Turn_Off(void)
{
	GPIO_SetOutBits(OUT_PC14_GPIO_PORT,OUT_PC14_GPIO_PIN);
}


/** ��תLED״̬
 */
void LED1_Toggle(void)
{
//    HT_GPIO_TypeDef* HT_GPIOB = HT_GPIOB;
//    HT_GPIO_TypeDef* HT_GPIOC = HT_GPIOC;//�Ѿ��������

    // ��ȡPC14���ŵ����״̬
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


