//-----------------------------------------------------------------------------
#ifndef __USART0_H
#define __USART0_H


//-----------------------------------------------------------------------------
#include "ht32.h"

//-----------------------------------------------------------------------------
#define USART0_BUF_SIZE 256
#define USART0_FIFO_LEN 1

//user define
#define CARD_ID_LENGTH 13  //���ų���
#define MAX_BOOKS 20 



//-----------------------------------------------------------------------------
typedef struct
{
  u8 buffer[USART0_BUF_SIZE];
  u16 write_pt;
  u16 read_pt;
  u16 cnt;
}_USART0_STRUCT;

//-----------------------------------------------------------------------------
void USART0_Configuration(void);
void USART0_init_buffer (void);
void USART0_analyzer_data(void);
void USART0_tx_data(u8 *pt, u8 len);
void USART0_test(void);

//-------------------------------------------------
//user code
void USART0_SendByte(u8 Byte);
void USART0_SendString(const char *str);
u8 USART0_GetRxFlag(void);

extern u8 USART0_TxPacket[CARD_ID_LENGTH];	//���巢�����ݰ����飬���ݰ���ʽ��FF 01 02 03 04 FE
extern u8 USART0_RxPacket[CARD_ID_LENGTH];	//����������ݰ�����
extern u8 USART0_RxFlag;					//����������ݰ���־λ
extern uint8_t id;


//test code
extern u8 test_flag;

//-----------------------------------------------------------------------------
#endif

