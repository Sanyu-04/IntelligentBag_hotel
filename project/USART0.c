//-----------------------------------------------------------------------------
#include "USART0.h"
#include "USART1.h"

//user define
u8 USART0_TxPacket[CARD_ID_LENGTH];	//定义发送数据包数组，数据包格式：FF 01 02 03 04 FE
u8 USART0_RxPacket[CARD_ID_LENGTH];	//定义接收数据包数组
u8 USART0_RxFlag;					//定义接收数据包标志位


//test code
u8 test_flag=0;


//-----------------------------------------------------------------------------
__ALIGN4 _USART0_STRUCT rxd_scomm0;
__ALIGN4 _USART0_STRUCT txd_scomm0;

//-----------------------------------------------------------------------------
void USART0_Configuration(void)
{
  USART_InitTypeDef USART_InitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.USART0   = 1;
  CKCUClock.Bit.AFIO     = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_USART_UART);  // Config AFIO mode
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_USART_UART);  // Config AFIO mode

  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStruct.USART_StopBits = USART_STOPBITS_1;
  USART_InitStruct.USART_Parity = USART_PARITY_NO;
  USART_InitStruct.USART_Mode = USART_MODE_NORMAL;
  USART_Init(HT_USART0, &USART_InitStruct);
  USART_RxCmd(HT_USART0, ENABLE);
  USART_TxCmd(HT_USART0, ENABLE);

  USART_RXTLConfig(HT_USART0, USART_RXTL_04);
  USART_TXTLConfig(HT_USART0, USART_TXTL_04);
  USART_IntConfig(HT_USART0, USART_INT_RXDR | USART_INT_TOUT, ENABLE);
  HT_USART0->TPR = 0x80 | 40;

  NVIC_EnableIRQ(USART0_IRQn);
  USART0_init_buffer();
}

//-----------------------------------------------------------------------------
void USART0_init_buffer (void)
{
  rxd_scomm0.read_pt = 0;
  rxd_scomm0.write_pt = 0;
  rxd_scomm0.cnt = 0;

  txd_scomm0.read_pt = 0;
  txd_scomm0.write_pt = 0;
  txd_scomm0.cnt = 0;
}

//-----------------------------------------------------------------------------
void USART0_IRQHandler(void)
{
    static uint8_t RxState = 0; 	// 定义表示当前状态机状态
    static uint8_t pRxPacket = 0; 	// 定义表示当前接收数据位
	
    // 判断是否是 USART0 的接收数据寄存器非空中断触发
    if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(HT_USART0); // 读取数据寄存器，存放在接收的数据变量
//		USART0_SendByte(RxData);
		test_flag=1;
        // 使用状态机的思路，依次处理数据包的不同部分 

        //当前状态为 0，接收数据包包头
        if (RxState == 0)
        {
            if (RxData == 0x02) // 如果数据确实是包头
            {
                RxState = 1; // 置下一个状态
                pRxPacket = 0; // 数据包的位置归零
            }
        }
        //当前状态为 1，接收数据包数据
        else if (RxState == 1)
        {
            USART0_RxPacket[pRxPacket] = RxData; // 将数据存入数据包数组的指定位置
            pRxPacket++; // 数据包的位置自增
            if (pRxPacket >= (CARD_ID_LENGTH - 2)) // 如果收够 13 个数据
            {
                RxState = 2; // 置下一个状态
            }
        }
        // 当前状态为 2，接收数据包包尾
        else if (RxState == 2)
        {
            if (RxData == 0x03) // 如果数据确实是包尾部
            {
                RxState = 0; // 状态归 0
                USART0_RxFlag = 1; // 接收数据包标志位置 1，成功接收一个数据包
            }
        }
    }
}


/*//初始化的中断函数(我觉得不好用换掉了)
void USART0_IRQHandler(void)
{
  // Tx, move data from buffer to UART FIFO
  if ((HT_USART0->SR) & USART_FLAG_TXC)
  {
    if (!txd_scomm0.cnt)
    {
      USART_IntConfig(HT_USART0, USART_INT_TXDE, DISABLE);
    }
    else
    {
      u16 i;
      for (i = 0; i < USART0_FIFO_LEN; i++)   //have FIFO?
      {
        USART_SendData(HT_USART0, txd_scomm0.buffer[txd_scomm0.read_pt]);
        txd_scomm0.read_pt = (txd_scomm0.read_pt + 1) % USART0_BUF_SIZE;
        txd_scomm0.cnt--;
        if (!txd_scomm0.cnt)
          break;
      }
    }
  }

  // Rx, move data from UART FIFO to buffer
  if ((HT_USART0->SR) & USART_FLAG_RXDR)
  {
    while(USART_GetFIFOStatus(HT_USART0, USART_FIFO_RX))
    {
      rxd_scomm0.buffer[rxd_scomm0.write_pt] = USART_ReceiveData(HT_USART0);

      rxd_scomm0.write_pt = (rxd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
      rxd_scomm0.cnt++;
    }
  }

  if ((HT_USART0->SR) & USART_FLAG_TOUT)
  {
    USART_ClearFlag(HT_USART0, USART_FLAG_TOUT);
    while(USART_GetFIFOStatus(HT_USART0, USART_FIFO_RX))
    {
      rxd_scomm0.buffer[rxd_scomm0.write_pt] = USART_ReceiveData(HT_USART0);

      rxd_scomm0.write_pt = (rxd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
      rxd_scomm0.cnt++;
    }
  }
}
*/

//-----------------------------------------------------------------------------
void USART0_analyzer_data(void)
{
  u8 tmp;
  if(!rxd_scomm0.cnt)
    return;

  //...to be add
  tmp = rxd_scomm0.buffer[rxd_scomm0.read_pt];
  if( (tmp != 0xAA) )            //for example
  {
    NVIC_DisableIRQ(USART0_IRQn);
    rxd_scomm0.cnt--;
    NVIC_EnableIRQ(USART0_IRQn);
    rxd_scomm0.read_pt = (rxd_scomm0.read_pt + 1) % USART0_BUF_SIZE;
    return;
  }
  else if(rxd_scomm0.cnt >= 8)    //for example
  {
    rxd_scomm0.cnt--;
    rxd_scomm0.read_pt = (rxd_scomm0.read_pt + 1) % USART0_BUF_SIZE;
  }
}

//-----------------------------------------------------------------------------
void USART0_tx_data(u8 *pt, u8 len)
{
  while(len--)
  {
    txd_scomm0.buffer[txd_scomm0.write_pt] = *pt++;
    txd_scomm0.write_pt = (txd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
    NVIC_DisableIRQ(USART0_IRQn);
    txd_scomm0.cnt++;
    NVIC_EnableIRQ(USART0_IRQn);
  }

  if(txd_scomm0.cnt)
    USART_IntConfig(HT_USART0,  USART_INT_TXDE , ENABLE);
}

//-----------------------------------------------------------------------------
void USART0_test(void)
{
  u8 i,test_array[8];
  for(i=0; i<8; i++)
  {
    test_array[i] = i;
  }
  USART0_tx_data(test_array, 8);
}

//-----------------------------------------------------------------------------


// //重定向c库函数printf到串口，重定向后可使用printf函数
//int fputc(int ch, FILE *f)
//{
//	/* 发送一个字节数据到串口 */
//	USART_SendData(HT_USART0, (uint8_t) ch);
//		
//	/* 等待发送完毕 */
//	while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXDE) == RESET);		
//	
//	return (ch);
//}
// 
// //重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
//int fgetc(FILE *f)
//{
//	/* 等待串口输入数据 */
//	while (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDNE) == RESET);
// 
//	return (int)USART_ReceiveData(HT_USART0);
//}




/**
  * 函    数：获取 USART0 串口接收数据包标志位
  * 参    数：无
  * 返 回 值：USART0串口接收数据包标志位，范围：0~1，接收到数据包后，标志位置1，读取后标志位自动清零
  */
u8 USART0_GetRxFlag(void)
{
    if (USART0_RxFlag == 1)  // 如果标志位为 1
    {        
        USART0_RxFlag = 0;
        return 1;  // 则返回 1，并自动清零标志位
    }
    return 0;  // 如果标志位为 0，则返回 0
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void USART0_SendByte(u8 Byte)
{
    //检查USART1是否是有效的USART外设
    Assert_Param(IS_USART(HT_USART0));
    // 发送数据到 USART1 的数据寄存器
    USART_SendData(HT_USART0, (u16)Byte);  
    // 等待发送完成，检查发送数据寄存器空标志位（USART_FLAG_TXDE）
    while(USART_GetFlagStatus(HT_USART0, USART_FLAG_TXDE) == RESET);  
    // 下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：str 要发送的字符串
  * 返 回 值：无
  */
void USART0_SendString(const char *str)
{
    if (str == NULL) {
        return;
    }
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        USART0_SendByte((u8)str[i]);
    }
}
