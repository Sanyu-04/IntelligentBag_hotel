//-----------------------------------------------------------------------------
#ifndef __USART1_H
#define __USART1_H


//-----------------------------------------------------------------------------
#include "ht32.h"
#include "USART0.h"
#include "WL-125.h"
//-----------------------------------------------------------------------------
#define USART1_BUF_SIZE 4
#define USART1_FIFO_LEN 1

//user define
#define CARD_ID_LENGTH 13  //���ų���
#define MAX_BOOKS 20 

//-----------------------------------------------------------------------------
typedef struct
{
  u8 buffer[USART1_BUF_SIZE];
  u16 write_pt;
  u16 read_pt;
  u16 cnt;
}_USART1_STRUCT;

//-----------------------------------------------------------------------------
void USART1_Configuration(void);
void USART1_init_buffer (void);
void USART1_analyzer_data(void);
void USART1_tx_data(u8 *pt, u8 len);
void USART1_test(void);

//user code
void USART1_SendByte(u8 Byte);
u8 USART1_GetRxFlag(void);

extern uint8_t rec_count;					//�α�̽�����ܽ��յĿα���
extern uint8_t ClassFlag;					//׼������RFID��־λ
extern uint8_t detect_flag;					//�α����־λ

extern u8 USART1_TxPacket[CARD_ID_LENGTH];	//���巢�����ݰ����飬���ݰ���ʽ��FF 01 02 03 04 FE
extern u8 USART1_RxPacket[CARD_ID_LENGTH];	//����������ݰ�����
extern u8 USART1_RxFlag;						//����������ݰ���־λ
extern __ALIGN4 _USART1_STRUCT rxd_scomm1;

extern uint8_t rec_flag;	//
extern uint8_t detect_buffer[100];
extern uint8_t rec_count;
extern uint8_t detect_flag;	//���ʶ���־λ

#define MAX_BOOK_NAME_LENGTH 50
#define MAX_COURSE_NUM 7
extern uint8_t book_name[MAX_BOOK_NAME_LENGTH];
// �µĶ�ά�������ڴ洢¼��� [book_name][����] ��Ϣ
typedef struct{
    uint8_t book_name[MAX_BOOK_NAME_LENGTH];
    uint8_t card_id[CARD_ID_LENGTH];
}CourseInfo;
extern CourseInfo course_info_table[MAX_COURSE_NUM];

// ����γ�������Ϣ
extern CourseInfo today_course_table[MAX_COURSE_NUM];
extern uint8_t today_course_count;



//-----------------------------------------------------------------------------
#endif

