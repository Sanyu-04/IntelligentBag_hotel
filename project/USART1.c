//-----------------------------------------------------------------------------
#include "USART1.h"
#include "string.h"


//-----------------------------------------------------------------------------
__ALIGN4 _USART1_STRUCT rxd_scomm1;
__ALIGN4 _USART1_STRUCT txd_scomm1;

//user define
u8 USART1_TxPacket[CARD_ID_LENGTH];	//���巢�����ݰ����飬���ݰ���ʽ��FF 01 02 03 04 FE
u8 USART1_RxPacket[CARD_ID_LENGTH];	//����������ݰ�����
u8 USART1_RxFlag;					//����������ݰ���־λ
uint8_t ClassFlag = 0;
uint8_t detect_flag = 0;
uint8_t rec_flag = 0;
uint8_t detect_buffer[100];
uint8_t rec_count = 0;

uint8_t book_name[MAX_BOOK_NAME_LENGTH];
//¼��γ���Ϣ����
CourseInfo course_info_table[MAX_COURSE_NUM];
// ����γ�����
CourseInfo today_course_table[MAX_COURSE_NUM];
uint8_t today_course_count = 0;

//test code
char debug_str[200];


//-----------------------------------------------------------------------------
void USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  // ʹ�� USART1 �� AFIO ��ʱ��
  CKCUClock.Bit.USART1   = 1;
  CKCUClock.Bit.AFIO     = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  
	// ���� PA4 �� PA5 ����Ϊ USART ����
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_USART_UART);  // Config AFIO mode
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_5, AFIO_FUN_USART_UART);  // Config AFIO mode

  // ���� USART1 �Ĳ���
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStruct.USART_StopBits = USART_STOPBITS_1;
  USART_InitStruct.USART_Parity = USART_PARITY_NO;
  USART_InitStruct.USART_Mode = USART_MODE_NORMAL;
  USART_Init(HT_USART1, &USART_InitStruct);// ��ʼ�� USART1
  // ʹ�� USART1 �Ľ��պͷ��͹���
  USART_RxCmd(HT_USART1, ENABLE);
  USART_TxCmd(HT_USART1, ENABLE);

  // ���� USART1 �Ľ��պͷ��� FIFO ��ֵ
  USART_RXTLConfig(HT_USART1, USART_RXTL_04);
  USART_TXTLConfig(HT_USART1, USART_TXTL_04);
   
   // ʹ�� USART1 �Ľ������ݼĴ����ǿ��жϺͽ��ճ�ʱ�ж�
  USART_IntConfig(HT_USART1, USART_INT_RXDR | USART_INT_TOUT, ENABLE);
  HT_USART1->TPR = 0x80 | 40;

  // ʹ�� USART1 ���ж�
  NVIC_EnableIRQ(USART1_IRQn);
  // ��ʼ�� USART1 �Ļ�����
  USART1_init_buffer();
  
}

//-----------------------------------------------------------------------------
// ��ʼ�� USART1 �Ľ��պͷ��ͻ�����
void USART1_init_buffer (void)
{
  // ��ʼ�����ջ�����
  rxd_scomm1.read_pt = 0;  // ���ջ�������ָ���ʼ��Ϊ 0
  rxd_scomm1.write_pt = 0;  // ���ջ�����дָ���ʼ��Ϊ 0
  rxd_scomm1.cnt = 0;  // ���ջ��������ݼ�����ʼ��Ϊ 0

  // ��ʼ�����ͻ�����
  txd_scomm1.read_pt = 0;  // ���ͻ�������ָ���ʼ��Ϊ 0
  txd_scomm1.write_pt = 0;  // ���ͻ�����дָ���ʼ��Ϊ 0
  txd_scomm1.cnt = 0;  // ���ͻ��������ݼ�����ʼ��Ϊ 0
}

//-----------------------------------------------------------------------------

//void asr_today_course(void){

//	for(int i =0;i<7;i++){
//	printf("%s\r\n",today_course_table[i].book_name);
//		if(strcmp((const char *)today_course_table[i].book_name,"����\0")){
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
		 // �����İ����ж��߼�
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
                book_name_index = 0;  // ����book_name����
            }
            else if(RxData == 0x82)	//���������̡����һ����־λ֪ͨ����
            {
                rec_flag = 2;
                today_course_count = 0;  // ���֮ǰ�ĵ���α��������
                memset(today_course_table, 0, sizeof(today_course_table)); // ��յ���α���������
            }
            break;
        case 1:
            if(RxData == 0xff)
            {
                book_name[book_name_index] = '\0'; // ���� book_name �ַ���
                // �յ����� book_name ���ñ�־λ
                is_inputting = 1;
                rec_flag = 0; // ���ý��ձ�־
                printf("�ѽ��������α�����%s\n", book_name);
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
                // ƥ�䲢�洢������γ�����
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
                rec_flag = 0; // ���ڿα���ģʽ������ rec_flag
                printf("���Ϳα����\n");
				print_today_course_table();
				printf("��ӡ���տα����\n");
				detect_flag = 1;	//ִ�м��α�
				
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
 //�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ����� */
	USART_SendData(HT_USART1, (uint8_t) ch);
		
	/* �ȴ�������� */
	while(USART_GetFlagStatus(HT_USART1, USART_FLAG_TXDE) == RESET);		
	
	return(ch);
}
 
	//�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
	/* �ȴ������������� */
	while(USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDNE) == RESET);
 
	return(int)USART_ReceiveData(HT_USART1);
}

//------------------------------------------------------------------------------
/*
//��ʼ�����жϺ���
//void USART1_IRQHandler(void)
//{
//  // Tx, move data from buffer to UART FIFO
//  // �����жϴ���
////  if ((HT_USART1->SR) & USART_FLAG_TXC)
////  {
////    // ������ͻ�����û������
////    if (!txd_scomm1.cnt)
////    {
////      // ���÷������ݼĴ������ж�
////      USART_IntConfig(HT_USART1, USART_INT_TXDE, DISABLE);
////    }
////    else
////    {
////      u16 i;
////      // �����ͻ�����������д�� UART FIFO
////      for (i = 0; i < USART1_FIFO_LEN; i++)   //have FIFO?
////      {
////        USART_SendData(HT_USART1, txd_scomm1.buffer[txd_scomm1.read_pt]);
////        txd_scomm1.read_pt = (txd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
////        txd_scomm1.cnt--;
////        // ������ͻ�����û�������ˣ��˳�ѭ��
////        if (!txd_scomm1.cnt)
////          break;
////      }
////    }
////  }

//  // Rx, move data from UART FIFO to buffer
//  // �������ݼĴ����ǿ��жϴ���
//  if ((HT_USART1->SR) & USART_FLAG_RXDNE)
//  {
//    // �� UART FIFO �е����ݶ�ȡ�����ջ�����
//    while(USART_GetFIFOStatus(HT_USART1, USART_FIFO_RX))
//    {
//      rxd_scomm1.buffer[rxd_scomm1.write_pt] = USART_ReceiveData(HT_USART1);
//      rxd_scomm1.write_pt = (rxd_scomm1.write_pt + 1) % USART1_BUF_SIZE;
//      rxd_scomm1.cnt++;
//    }
//  } 
//  // ���ճ�ʱ�жϴ���
////  if ((HT_USART1->SR) & USART_FLAG_TOUT)
////  {
////    // ������ճ�ʱ��־
////    USART_ClearFlag(HT_USART1, USART_FLAG_TOUT);
////    // �� UART FIFO �е����ݶ�ȡ�����ջ�����
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
// �������յ�������
void USART1_analyzer_data(void)
{
  u8 tmp;
  // ������ջ�����û�����ݣ�ֱ�ӷ���
  if(!rxd_scomm1.cnt)
    return;

  //...to be add
  // ��ȡ���ջ������ĵ�һ������
  tmp = rxd_scomm1.buffer[rxd_scomm1.read_pt];
  // �����һ�����ݲ��� 0xAA
  if( (tmp != 0xAA) )            //for example
  {
    // ���� USART1 �ж�
    NVIC_DisableIRQ(USART1_IRQn);
    rxd_scomm1.cnt--;
    // ʹ�� USART1 �ж�
    NVIC_EnableIRQ(USART1_IRQn);
    rxd_scomm1.read_pt = (rxd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
    return;
  }
  // ������ջ����������ݳ��ȴ��ڵ��� 8
  else if(rxd_scomm1.cnt >= 8)    //for example
  {
    rxd_scomm1.cnt--;
    rxd_scomm1.read_pt = (rxd_scomm1.read_pt + 1) % USART1_BUF_SIZE;
  }
}
//-----------------------------------------------------------------------------
// �� USART1 ��������
void USART1_tx_data(u8 *pt, u8 len)
{
  // ������д�뷢�ͻ�����
  while(len--)
  {
    txd_scomm1.buffer[txd_scomm1.write_pt] = *pt++;
    txd_scomm1.write_pt = (txd_scomm1.write_pt + 1) % USART1_BUF_SIZE;
    // ���� USART1 �ж�
    NVIC_DisableIRQ(USART1_IRQn);
    txd_scomm1.cnt++;
    // ʹ�� USART1 �ж�
    NVIC_EnableIRQ(USART1_IRQn);
  }

  // ������ͻ����������ݣ�ʹ�ܷ������ݼĴ������ж�
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
  * ��    ������ȡ USART1 ���ڽ������ݰ���־λ
  * ��    ������
  * �� �� ֵ��USART1 ���ڽ������ݰ���־λ����Χ��0~1�����յ����ݰ��󣬱�־λ��1����ȡ���־λ�Զ�����
  */
u8 USART1_GetRxFlag(void)
{
    if (USART1_RxFlag == 1)  // �����־λΪ 1
    {
        USART1_RxFlag = 0;
        return 1;  // �򷵻� 1�����Զ������־λ
    }
    return 0;  // �����־λΪ 0���򷵻� 0
}

/**
  * ��    �������ڷ���һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void USART1_SendByte(u8 Byte)
{
    // ��� USART1 �Ƿ�����Ч�� USART ���裨������� HT_USART1 ����Ч�ģ�ʵ��ʹ���пɸ������������
    Assert_Param(IS_USART(HT_USART1));
    // �������ݵ� USART1 �����ݼĴ���
    USART_SendData(HT_USART1, (u16)Byte);  
    // �ȴ�������ɣ���鷢�����ݼĴ����ձ�־λ��USART_FLAG_TXDE��
    while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXDE) == RESET);  
    // �´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ
}


