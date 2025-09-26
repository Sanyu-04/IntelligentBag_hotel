//-----------------------------------------------------------------------------
#include "USART1.h"
#include "string.h"


//-----------------------------------------------------------------------------
__ALIGN4 _USART1_STRUCT rxd_scomm1;
__ALIGN4 _USART1_STRUCT txd_scomm1;

//user define
u8 USART1_TxPacket[CARD_ID_LENGTH];	//定义发送数据包数组，数据包格式：FF 01 02 03 04 FE
u8 USART1_RxPacket[CARD_ID_LENGTH];	//定义接收数据包数组
u8 USART1_RxFlag;					//定义接收数据包标志位
uint8_t ClassFlag = 0;
uint8_t detect_flag = 0;
uint8_t rec_flag = 0;
uint8_t detect_buffer[100];
uint8_t rec_count = 0;

uint8_t book_name[MAX_BOOK_NAME_LENGTH];
//录入课程信息数组
CourseInfo course_info_table[MAX_COURSE_NUM];
// 当天课程数组
CourseInfo today_course_table[MAX_COURSE_NUM];
uint8_t today_course_count = 0;

//test code
char debug_str[200];


//-----------------------------------------------------------------------------
void USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  // 使能 USART1 和 AFIO 的时钟
  CKCUClock.Bit.USART1   = 1;
  CKCUClock.Bit.AFIO     = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  
	// 配置 PA4 和 PA5 引脚为 USART 功能
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_USART_UART);  // Config AFIO mode
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_5, AFIO_FUN_USART_UART);  // Config AFIO mode

  // 配置 USART1 的参数
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStruct.USART_StopBits = USART_STOPBITS_1;
  USART_InitStruct.USART_Parity = USART_PARITY_NO;
  USART_InitStruct.USART_Mode = USART_MODE_NORMAL;
  USART_Init(HT_USART1, &USART_InitStruct);// 初始化 USART1
  // 使能 USART1 的接收和发送功能
  USART_RxCmd(HT_USART1, ENABLE);
  USART_TxCmd(HT_USART1, ENABLE);

  // 配置 USART1 的接收和发送 FIFO 阈值
  USART_RXTLConfig(HT_USART1, USART_RXTL_04);
  USART_TXTLConfig(HT_USART1, USART_TXTL_04);
   
   // 使能 USART1 的接收数据寄存器非空中断和接收超时中断
  USART_IntConfig(HT_USART1, USART_INT_RXDR | USART_INT_TOUT, ENABLE);
  HT_USART1->TPR = 0x80 | 40;

  // 使能 USART1 的中断
  NVIC_EnableIRQ(USART1_IRQn);
  // 初始化 USART1 的缓冲区
  USART1_init_buffer();
  
}

//-----------------------------------------------------------------------------
// 初始化 USART1 的接收和发送缓冲区
void USART1_init_buffer (void)
{
  // 初始化接收缓冲区
  rxd_scomm1.read_pt = 0;  // 接收缓冲区读指针初始化为 0
  rxd_scomm1.write_pt = 0;  // 接收缓冲区写指针初始化为 0
  rxd_scomm1.cnt = 0;  // 接收缓冲区数据计数初始化为 0

  // 初始化发送缓冲区
  txd_scomm1.read_pt = 0;  // 发送缓冲区读指针初始化为 0
  txd_scomm1.write_pt = 0;  // 发送缓冲区写指针初始化为 0
  txd_scomm1.cnt = 0;  // 发送缓冲区数据计数初始化为 0
}

//-----------------------------------------------------------------------------

//void asr_today_course(void){

//	for(int i =0;i<7;i++){
//	printf("%s\r\n",today_course_table[i].book_name);
//		if(strcmp((const char *)today_course_table[i].book_name,"语文\0")){
//			printf("aaa");
//		}
//	
//	}

//}

//-----------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
	static uint8_t book_name_index = 0;
    if(USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(HT_USART1);
		 // 新增的按键判断逻辑
        if(RxData == 0x33)
        {
            key_number = 'a';
        }
        else if(RxData == 0x44)
        {
            key_number = 'b';
        }
        Find_key();
		
        switch(rec_flag)
        {
        case 0:
            if(RxData == 0x81)
            {
                rec_flag = 1;
                ClassFlag = 1;
                book_name_index = 0;  // 重置book_name索引
            }
            else if(RxData == 0x82)	//进入检测流程。添加一个标志位通知语音
            {
                rec_flag = 2;
                today_course_count = 0;  // 清空之前的当天课表数组计数
                memset(today_course_table, 0, sizeof(today_course_table)); // 清空当天课表数组内容
            }
            break;
        case 1:
            if(RxData == 0xff)
            {
                book_name[book_name_index] = '\0'; // 结束 book_name 字符串
                // 收到完整 book_name 后，置标志位
                is_inputting = 1;
                rec_flag = 0; // 重置接收标志
                printf("已接收完整课本名：%s\n", book_name);
            }
            else
            {
                if(book_name_index < MAX_BOOK_NAME_LENGTH - 1)
                {
                    book_name[book_name_index++] = RxData;
                }
            }
            break;
        case 2:
            if(RxData == 0x28)            {
                detect_buffer[rec_count] = '\0';
                // 匹配并存储到当天课程数组
                for(uint8_t i = 0; i < MAX_COURSE_NUM; i++)
                {
                    if(strcmp((char*)detect_buffer, (char*)course_info_table[i].book_name) == 0)
                    {
                        strcpy((char*)today_course_table[today_course_count].book_name, (char*)course_info_table[i].book_name);
                        strcpy((char*)today_course_table[today_course_count].card_id, (char*)course_info_table[i].card_id);
                        today_course_count++;
                        break;
                    }
                }
                rec_count = 0;
            }
            else if(RxData == 0x18)
            {
                rec_flag = 0; // 仅在课表检测模式下重置 rec_flag
                printf("发送课表结束\n");
				print_today_course_table();
				printf("打印当日课表结束\n");
				detect_flag = 1;	//执行检测课表
				
            }
            else
            {
                if(rec_count < sizeof(detect_buffer) - 1)
                {
                    detect_buffer[rec_count++] = RxData;
                }
            }
            break;
        }
    }
}



//-------------------------------------------------------------------------
 //重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(HT_USART1, (uint8_t) ch);
		
	/* 等待发送完毕 */
	while(USART_GetFlagStatus(HT_USART1, USART_FLAG_TXDE) == RESET);		
	
	return(ch);
}
 
	//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while(USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDNE) == RESET);
 
	return(int)USART_ReceiveData(HT_USART1);
}

//------------------------------------------------------------------------------
/*
//初始化的中断函数
//void USART1_IRQHandler(void)
//{
//  // Tx, move data from buffer to UART FIFO
//  // 发送中断处理
////  if ((HT_USART1->SR) & USART_FLAG_TXC)
////  {
////    // 如果发送缓冲区没有数据
////    if (!txd_scomm1.cnt)
////    {
////      // 禁用发送数据寄存器空中断
////      USART_IntConfig(HT_USART1, USART_INT_TXDE, DISABLE);
////    }
////    else
////    {
////      u16 i;
////      // 将发送缓冲区的数据写入 UART FIFO
////      for (i = 0; i < USART1_FIFO_LEN; i++)   //have FIFO?
////      {
////        USART_SendData(HT_USART1, txd_scomm1.buffer[txd_scomm1.read_pt]);
////        txd_scomm1.read_pt = (txd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
////        txd_scomm1.cnt--;
////        // 如果发送缓冲区没有数据了，退出循环
////        if (!txd_scomm1.cnt)
////          break;
////      }
////    }
////  }

//  // Rx, move data from UART FIFO to buffer
//  // 接收数据寄存器非空中断处理
//  if ((HT_USART1->SR) & USART_FLAG_RXDNE)
//  {
//    // 将 UART FIFO 中的数据读取到接收缓冲区
//    while(USART_GetFIFOStatus(HT_USART1, USART_FIFO_RX))
//    {
//      rxd_scomm1.buffer[rxd_scomm1.write_pt] = USART_ReceiveData(HT_USART1);
//      rxd_scomm1.write_pt = (rxd_scomm1.write_pt + 1) % USART1_BUF_SIZE;
//      rxd_scomm1.cnt++;
//    }
//  } 
//  // 接收超时中断处理
////  if ((HT_USART1->SR) & USART_FLAG_TOUT)
////  {
////    // 清除接收超时标志
////    USART_ClearFlag(HT_USART1, USART_FLAG_TOUT);
////    // 将 UART FIFO 中的数据读取到接收缓冲区
////    while(USART_GetFIFOStatus(HT_USART1, USART_FIFO_RX))
////    {
////      rxd_scomm1.buffer[rxd_scomm1.write_pt] = USART_ReceiveData(HT_USART1);
////      rxd_scomm1.write_pt = (rxd_scomm1.write_pt + 1) % USART1_BUF_SIZE;
////      rxd_scomm1.cnt++;
////    }
////  }
//}
*/

//-----------------------------------------------------------------------------
// 分析接收到的数据
void USART1_analyzer_data(void)
{
  u8 tmp;
  // 如果接收缓冲区没有数据，直接返回
  if(!rxd_scomm1.cnt)
    return;

  //...to be add
  // 读取接收缓冲区的第一个数据
  tmp = rxd_scomm1.buffer[rxd_scomm1.read_pt];
  // 如果第一个数据不是 0xAA
  if( (tmp != 0xAA) )            //for example
  {
    // 禁用 USART1 中断
    NVIC_DisableIRQ(USART1_IRQn);
    rxd_scomm1.cnt--;
    // 使能 USART1 中断
    NVIC_EnableIRQ(USART1_IRQn);
    rxd_scomm1.read_pt = (rxd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
    return;
  }
  // 如果接收缓冲区的数据长度大于等于 8
  else if(rxd_scomm1.cnt >= 8)    //for example
  {
    rxd_scomm1.cnt--;
    rxd_scomm1.read_pt = (rxd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
  }
}
//-----------------------------------------------------------------------------
// 向 USART1 发送数据
void USART1_tx_data(u8 *pt, u8 len)
{
  // 将数据写入发送缓冲区
  while(len--)
  {
    txd_scomm1.buffer[txd_scomm1.write_pt] = *pt++;
    txd_scomm1.write_pt = (txd_scomm1.write_pt + 1) % USART1_BUF_SIZE;
    // 禁用 USART1 中断
    NVIC_DisableIRQ(USART1_IRQn);
    txd_scomm1.cnt++;
    // 使能 USART1 中断
    NVIC_EnableIRQ(USART1_IRQn);
  }

  // 如果发送缓冲区有数据，使能发送数据寄存器空中断
  if(txd_scomm1.cnt)
    USART_IntConfig(HT_USART1,  USART_INT_TXDE , ENABLE);
}

//-----------------------------------------------------------------------------
void USART1_test(void)
{
  u8 i,test_array[8];
  for(i=0; i<8; i++)
  {
    test_array[i] = i;
  }
  USART1_tx_data(test_array, 8);
}

//-----------------------------------------------------------------------------





/**
  * 函    数：获取 USART1 串口接收数据包标志位
  * 参    数：无
  * 返 回 值：USART1 串口接收数据包标志位，范围：0~1，接收到数据包后，标志位置1，读取后标志位自动清零
  */
u8 USART1_GetRxFlag(void)
{
    if (USART1_RxFlag == 1)  // 如果标志位为 1
    {
        USART1_RxFlag = 0;
        return 1;  // 则返回 1，并自动清零标志位
    }
    return 0;  // 如果标志位为 0，则返回 0
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void USART1_SendByte(u8 Byte)
{
    // 检查 USART1 是否是有效的 USART 外设（这里假设 HT_USART1 是有效的，实际使用中可根据情况调整）
    Assert_Param(IS_USART(HT_USART1));
    // 发送数据到 USART1 的数据寄存器
    USART_SendData(HT_USART1, (u16)Byte);  
    // 等待发送完成，检查发送数据寄存器空标志位（USART_FLAG_TXDE）
    while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXDE) == RESET);  
    // 下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位
}


