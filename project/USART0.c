//-----------------------------------------------------------------------------
#include "USART0.h"
#include "USART1.h"

//user define
u8 USART0_TxPacket[CARD_ID_LENGTH];	//���巢�����ݰ����飬���ݰ���ʽ��FF 01 02 03 04 FE
u8 USART0_RxPacket[CARD_ID_LENGTH];	//����������ݰ�����
u8 USART0_RxFlag;					//����������ݰ���־λ


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
    static uint8_t RxState = 0; 	// �����ʾ��ǰ״̬��״̬
    static uint8_t pRxPacket = 0; 	// �����ʾ��ǰ��������λ
	
    // �ж��Ƿ��� USART0 �Ľ������ݼĴ����ǿ��жϴ���
    if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(HT_USART0); // ��ȡ���ݼĴ���������ڽ��յ����ݱ���
//		USART0_SendByte(RxData);
		test_flag=1;
        // ʹ��״̬����˼·�����δ������ݰ��Ĳ�ͬ���� 

        //��ǰ״̬Ϊ 0���������ݰ���ͷ
        if (RxState == 0)
        {
            if (RxData == 0x02) // �������ȷʵ�ǰ�ͷ
            {
                RxState = 1; // ����һ��״̬
                pRxPacket = 0; // ���ݰ���λ�ù���
            }
        }
        //��ǰ״̬Ϊ 1���������ݰ�����
        else if (RxState == 1)
        {
            USART0_RxPacket[pRxPacket] = RxData; // �����ݴ������ݰ������ָ��λ��
            pRxPacket++; // ���ݰ���λ������
            if (pRxPacket >= (CARD_ID_LENGTH - 2)) // ����չ� 13 ������
            {
                RxState = 2; // ����һ��״̬
            }
        }
        // ��ǰ״̬Ϊ 2���������ݰ���β
        else if (RxState == 2)
        {
            if (RxData == 0x03) // �������ȷʵ�ǰ�β��
            {
                RxState = 0; // ״̬�� 0
                USART0_RxFlag = 1; // �������ݰ���־λ�� 1���ɹ�����һ�����ݰ�
            }
        }
    }
}


/*//��ʼ�����жϺ���(�Ҿ��ò����û�����)
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


// //�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
//int fputc(int ch, FILE *f)
//{
//	/* ����һ���ֽ����ݵ����� */
//	USART_SendData(HT_USART0, (uint8_t) ch);
//		
//	/* �ȴ�������� */
//	while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXDE) == RESET);		
//	
//	return (ch);
//}
// 
// //�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
//int fgetc(FILE *f)
//{
//	/* �ȴ������������� */
//	while (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDNE) == RESET);
// 
//	return (int)USART_ReceiveData(HT_USART0);
//}




/**
  * ��    ������ȡ USART0 ���ڽ������ݰ���־λ
  * ��    ������
  * �� �� ֵ��USART0���ڽ������ݰ���־λ����Χ��0~1�����յ����ݰ��󣬱�־λ��1����ȡ���־λ�Զ�����
  */
u8 USART0_GetRxFlag(void)
{
    if (USART0_RxFlag == 1)  // �����־λΪ 1
    {        
        USART0_RxFlag = 0;
        return 1;  // �򷵻� 1�����Զ������־λ
    }
    return 0;  // �����־λΪ 0���򷵻� 0
}

/**
  * ��    �������ڷ���һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void USART0_SendByte(u8 Byte)
{
    //���USART1�Ƿ�����Ч��USART����
    Assert_Param(IS_USART(HT_USART0));
    // �������ݵ� USART1 �����ݼĴ���
    USART_SendData(HT_USART0, (u16)Byte);  
    // �ȴ�������ɣ���鷢�����ݼĴ����ձ�־λ��USART_FLAG_TXDE��
    while(USART_GetFlagStatus(HT_USART0, USART_FLAG_TXDE) == RESET);  
    // �´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ
}

/**
  * ��    �������ڷ���һ���ַ���
  * ��    ����str Ҫ���͵��ַ���
  * �� �� ֵ����
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
