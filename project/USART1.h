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
#define CARD_ID_LENGTH 13  //卡号长度
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

extern uint8_t rec_count;					//课表探测中总接收的课本数
extern uint8_t ClassFlag;					//准备接收RFID标志位
extern uint8_t detect_flag;					//课表检测标志位

extern u8 USART1_TxPacket[CARD_ID_LENGTH];	//定义发送数据包数组，数据包格式：FF 01 02 03 04 FE
extern u8 USART1_RxPacket[CARD_ID_LENGTH];	//定义接收数据包数组
extern u8 USART1_RxFlag;						//定义接收数据包标志位
extern __ALIGN4 _USART1_STRUCT rxd_scomm1;

extern uint8_t rec_flag;	//
extern uint8_t detect_buffer[100];
extern uint8_t rec_count;
extern uint8_t detect_flag;	//检测识别标志位

#define MAX_BOOK_NAME_LENGTH 50
#define MAX_COURSE_NUM 7
extern uint8_t book_name[MAX_BOOK_NAME_LENGTH];
// 新的二维数组用于存储录入的 [book_name][卡号] 信息
typedef struct{
    uint8_t book_name[MAX_BOOK_NAME_LENGTH];
    uint8_t card_id[CARD_ID_LENGTH];
}CourseInfo;
extern CourseInfo course_info_table[MAX_COURSE_NUM];

// 当天课程数组信息
extern CourseInfo today_course_table[MAX_COURSE_NUM];
extern uint8_t today_course_count;



//-----------------------------------------------------------------------------
#endif

