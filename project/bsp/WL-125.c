#include "WL-125.h"
//char week[]="周五";
u8 course_count = 0;  		//当前课表中的课本数量
uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH] = {0};
uint8_t detected_count = 0;

// 新增标志位，用于控制录入循环
u8 is_inputting = 0;

//给语音模块发送缺失的书本
void Chinese_convert(const char * bookname){
	if(memcmp(bookname, "语文\0", 7) == 0){
		UART1_SendString("yw");
	}
	else if(memcmp(bookname, "数学\0", 7) == 0){
		UART1_SendString("sx");
	}
	else if(memcmp(bookname, "英语\0", 7) == 0){
		UART1_SendString("en");
	}
	else if(memcmp(bookname, "化学\0", 7) == 0){
		UART1_SendString("hx");
	}
	else if(memcmp(bookname, "物理\0", 7) == 0){
		UART1_SendString("wl");
	}
	else if(memcmp(bookname, "音乐\0", 7) == 0){
		UART1_SendString("yy");
	}
	else if(memcmp(bookname, "美术\0", 7) == 0){
		UART1_SendString("ms");
	}
}

// 清空当天课程数组
void init_today_course_array(void)
{
    today_course_count = 0;
    for(int i = 0; i < MAX_COURSE_NUM; i++) {
        for(int j = 0; j < MAX_BOOK_NAME_LENGTH; j++) {
            today_course_table[i].book_name[j] = '\0';
        }
        for(int k = 0; k < CARD_ID_LENGTH; k++) {
            today_course_table[i].card_id[k] = '\0';
        }
    }
}

// 这是打印已录入课本函数
void printf_course_table(void)
{
	USART0_SendString("开始打印课表\r\n");
    for(uint8_t i = 0; i < course_count; i++)
    {
        // 发送课本名
        for(int j = 0; course_info_table[i].book_name[j] != '\0'; j++)
        {
            USART0_SendByte(course_info_table[i].book_name[j]);

        }
			
        USART0_SendByte(':');
        // 发送卡号
        for(int k = 0; k < CARD_ID_LENGTH; k++)
        {
            USART0_SendByte(course_info_table[i].card_id[k]);
//            if(k < CARD_ID_LENGTH - 1)
//            {
//                USART0_SendByte(' ');
//            }
        }
        USART0_SendByte('\n');
    }
}
//打印当天课表
void print_today_course_table(void)
{
    USART0_SendString("开始打印当天课表\n");
    for(uint8_t i = 0; i < today_course_count; i++)
    {
        // 发送课本名
        for(int j = 0; today_course_table[i].book_name[j] != '\0'; j++)
        {
            USART0_SendByte(today_course_table[i].book_name[j]);
			
        }
		
		UART1_SendString("83");
USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");

		Chinese_convert((const char *)today_course_table[i].book_name);
USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");

		UART1_SendString("90");
USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
		
        USART0_SendByte(':');
        // 发送卡号
        for(int k = 0; k < CARD_ID_LENGTH; k++)
        {
            USART0_SendByte(today_course_table[i].card_id[k]);
//            if(k < CARD_ID_LENGTH - 1)
//            {
//                USART0_SendByte(' ');
//            }
        }
        USART0_SendByte('\n');
    }
}

//这是课本录入绑定id卡号的函数
void InputNewCourse(void)
{
//    course_count = 0;  // 清空课表
	
    while(is_inputting)
    {
		printf("接收绑定标签信息...\n");
		//等待USART0接收完卡号
		while(!USART0_GetRxFlag())
        {
               // 可以适当添加延时，避免空转占用过多 CPU 资源
//			vTaskDelay(10); 
		}
        int is_duplicate = 0;
        // 检查新标签是否已经存在于课程表中
        for(uint8_t i = 0; i < course_count; i++)
        {
            int match = 1;
            for(int j = 0; j < CARD_ID_LENGTH; j++)
            {
                if(course_info_table[i].card_id[j] != USART0_RxPacket[j])
                {
                    match = 0;
                    break;
                }
            }
            if(match)
            {
                is_duplicate = 1;
                break;
            }
        }

        if(!is_duplicate)
        {
            if(ClassFlag == 1)
            {
                // 复制 book_name 到新的数组
                for(int i = 0; i < MAX_BOOK_NAME_LENGTH; i++)
                {
                    course_info_table[course_count].book_name[i] = book_name[i];
                }
                // 复制卡号到新的数组
                for(int i = 0; i < CARD_ID_LENGTH; i++)
                {
                    course_info_table[course_count].card_id[i] = USART0_RxPacket[i];
                }
				
				/*//test code 调试输出确认卡号是否正确存储
                printf("存储后的卡号: ");
                for (int i = 0; i < CARD_ID_LENGTH; i++)
                {
                    printf("%02X ", course_info_table[course_count].card_id[i]);
                }
                printf("\r\n");
				*/
				
                
				printf("录入课本 %d: ", course_count);
                for (int i = 0; i < MAX_BOOK_NAME_LENGTH; i++)
                {
                    if(course_info_table[course_count].book_name[i] != '\0')
                    {
                        printf("%c", course_info_table[course_count].book_name[i]);
                    }
                }
                printf(" 卡号: ");
                for(int i = 0; i < CARD_ID_LENGTH; i++)
                {
                    printf("%02X ", course_info_table[course_count].card_id[i]);
                }
                printf("\r\n");
                course_count++;
            }
		}
		else
		{
			printf("该课本已存在，无法重复录入\n");
		}
		
		USART0_RxFlag = 0; // 重置标志位
		//绑定一本课本的卡号信息结束时置0
		ClassFlag = 0;     // 重置 ClassFlag 
		HMI_send_number("course.a.val",0);
		HMI_send_number("course.book1.bco",1024);
		HMI_send_number("course.book2.bco",1024);
		HMI_send_number("course.book3.bco",1024);
		HMI_send_number("course.book4.bco",1024);
		HMI_send_number("course.book5.bco",1024);
		HMI_send_number("course.book6.bco",1024);
		HMI_send_number("course.book7.bco",1024);
		HMI_send_number("course.aaa.val",2);
		HMI_send_number("course.b1.bco",1024);
		HMI_send_string("course.b1.txt","录入课本");
		printf_course_table();//进行已录入课本的信息打印
		is_inputting = 0;  
		break;
    }
}



  //5 秒课表检测识别
void CourseTableDetection(void)
{
    TickType_t start_time = xTaskGetTickCount(); // 使用 xTaskGetTickCount 获取起始时间
    TickType_t elapsed_time = 0;

    uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH] = {0};
    uint8_t detected_count = 0;

    uint8_t retry_count = 0;
    const uint8_t max_retries = 3; // 最大重试次数
	uint8_t found_flag = 0;
    while(elapsed_time < pdMS_TO_TICKS(10000))
	{
        taskENTER_CRITICAL(); // 进入临界区，防止其他任务打断
        // 增加重试机制
        for(retry_count = 0; retry_count < max_retries; retry_count++)
		{
            if(USART0_GetRxFlag())
			{
                int found = 0;
                // 检查是否已检测到该标签
                for (uint8_t i = 0; i < detected_count; i++) {
                    if (memcmp(detected_table[i], USART0_RxPacket, CARD_ID_LENGTH) == 0) {
                        found = 1;
                        break;
                    }
                }

                // 如果是新标签且未达到最大数量，则记录
                if (!found && detected_count < MAX_COURSE_NUM) {
                    memcpy(detected_table[detected_count], USART0_RxPacket, CARD_ID_LENGTH);
                    detected_count++;
                    retry_count = 0; // 重置重试计数器
                }
            }
            taskEXIT_CRITICAL(); // 退出临界区
            vTaskDelay(pdMS_TO_TICKS(10)); // 添加短延迟，减少冲突
            taskENTER_CRITICAL(); // 重新进入临界区
        }
        taskEXIT_CRITICAL(); // 退出临界区
        elapsed_time = xTaskGetTickCount() - start_time;
    }

    taskENTER_CRITICAL(); // 进入临界区，防止其他任务打断
    
	
	//使用USART0_SendString打印缺失课本
    USART0_SendString("缺失课本：\n");
	uint8_t miss_flag = 1;
    for(uint8_t i = 0; i < today_course_count; i++)
	{
        int found = 0;
        for(uint8_t j = 0; j < detected_count; j++)
		{
            if(memcmp(today_course_table[i].card_id, detected_table[j], CARD_ID_LENGTH) == 0)
			{
                found = 1;
                break;
            }
        }
        if(!found)
		{
            USART0_SendString((const char *)today_course_table[i].book_name);
            USART0_SendString("\n");
			
			found_flag = 1;
			
			if(miss_flag){
				miss_flag = 0;
			UART1_SendString("87");//缺少课本名称
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			UART1_SendString("90");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			}
			
			UART1_SendString("85");//缺少课本名称
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//			Delay_ms(100);
			Chinese_convert((const char *)today_course_table[i].book_name);
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
			UART1_SendString("90");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
        }

    }
	if (found_flag == 0){
			UART1_SendString("84");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
			UART1_SendByte('0');
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
			UART1_SendString("90");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
	}
	else if (found_flag == 1){
			UART1_SendString("84");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
			UART1_SendByte('1');
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
			UART1_SendString("90");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
			USART0_SendString("1111111");
//Delay_ms(100);
	}
	USART0_SendString("检测结束\n");//发送语音通知
	
//	UART1_SendString("缺失课本：\n");
//    for(uint8_t i = 0; i < today_course_count; i++)
//	{
//        int found = 0;
//        for(uint8_t j = 0; j < detected_count; j++)
//		{
//            if(memcmp(today_course_table[i].card_id, detected_table[j], CARD_ID_LENGTH) == 0)
//			{
//                found = 1;
//                break;
//            }
//        }
//        if(!found)
//		{
//            UART1_SendString((char *)today_course_table[i].book_name);
//            UART1_SendString("\n");
//        }
//    }
//	UART1_SendString("检测结束\n");
//	
	HMI_send_number("course_table.b2.bco",65535);
	HMI_send_number("course_table.b2.bco2",65535);
	HMI_send_string("course_table.b2.txt","检测课本");
	
	detect_flag=0;	//表示检测结束
    taskEXIT_CRITICAL(); // 退出临界区
}


