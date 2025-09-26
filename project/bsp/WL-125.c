#include "WL-125.h"
//char week[]="����";
u8 course_count = 0;  		//��ǰ�α��еĿα�����
uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH] = {0};
uint8_t detected_count = 0;

// ������־λ�����ڿ���¼��ѭ��
u8 is_inputting = 0;

//������ģ�鷢��ȱʧ���鱾
void Chinese_convert(const char * bookname){
	if(memcmp(bookname, "����\0", 7) == 0){
		UART1_SendString("yw");
	}
	else if(memcmp(bookname, "��ѧ\0", 7) == 0){
		UART1_SendString("sx");
	}
	else if(memcmp(bookname, "Ӣ��\0", 7) == 0){
		UART1_SendString("en");
	}
	else if(memcmp(bookname, "��ѧ\0", 7) == 0){
		UART1_SendString("hx");
	}
	else if(memcmp(bookname, "����\0", 7) == 0){
		UART1_SendString("wl");
	}
	else if(memcmp(bookname, "����\0", 7) == 0){
		UART1_SendString("yy");
	}
	else if(memcmp(bookname, "����\0", 7) == 0){
		UART1_SendString("ms");
	}
}

// ��յ���γ�����
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

// ���Ǵ�ӡ��¼��α�����
void printf_course_table(void)
{
	USART0_SendString("��ʼ��ӡ�α�\r\n");
    for(uint8_t i = 0; i < course_count; i++)
    {
        // ���Ϳα���
        for(int j = 0; course_info_table[i].book_name[j] != '\0'; j++)
        {
            USART0_SendByte(course_info_table[i].book_name[j]);

        }
			
        USART0_SendByte(':');
        // ���Ϳ���
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
//��ӡ����α�
void print_today_course_table(void)
{
    USART0_SendString("��ʼ��ӡ����α�\n");
    for(uint8_t i = 0; i < today_course_count; i++)
    {
        // ���Ϳα���
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
        // ���Ϳ���
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

//���ǿα�¼���id���ŵĺ���
void InputNewCourse(void)
{
//    course_count = 0;  // ��տα�
	
    while(is_inputting)
    {
		printf("���հ󶨱�ǩ��Ϣ...\n");
		//�ȴ�USART0�����꿨��
		while(!USART0_GetRxFlag())
        {
               // �����ʵ������ʱ�������תռ�ù��� CPU ��Դ
//			vTaskDelay(10); 
		}
        int is_duplicate = 0;
        // ����±�ǩ�Ƿ��Ѿ������ڿγ̱���
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
                // ���� book_name ���µ�����
                for(int i = 0; i < MAX_BOOK_NAME_LENGTH; i++)
                {
                    course_info_table[course_count].book_name[i] = book_name[i];
                }
                // ���ƿ��ŵ��µ�����
                for(int i = 0; i < CARD_ID_LENGTH; i++)
                {
                    course_info_table[course_count].card_id[i] = USART0_RxPacket[i];
                }
				
				/*//test code �������ȷ�Ͽ����Ƿ���ȷ�洢
                printf("�洢��Ŀ���: ");
                for (int i = 0; i < CARD_ID_LENGTH; i++)
                {
                    printf("%02X ", course_info_table[course_count].card_id[i]);
                }
                printf("\r\n");
				*/
				
                
				printf("¼��α� %d: ", course_count);
                for (int i = 0; i < MAX_BOOK_NAME_LENGTH; i++)
                {
                    if(course_info_table[course_count].book_name[i] != '\0')
                    {
                        printf("%c", course_info_table[course_count].book_name[i]);
                    }
                }
                printf(" ����: ");
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
			printf("�ÿα��Ѵ��ڣ��޷��ظ�¼��\n");
		}
		
		USART0_RxFlag = 0; // ���ñ�־λ
		//��һ���α��Ŀ�����Ϣ����ʱ��0
		ClassFlag = 0;     // ���� ClassFlag 
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
		HMI_send_string("course.b1.txt","¼��α�");
		printf_course_table();//������¼��α�����Ϣ��ӡ
		is_inputting = 0;  
		break;
    }
}



  //5 ��α���ʶ��
void CourseTableDetection(void)
{
    TickType_t start_time = xTaskGetTickCount(); // ʹ�� xTaskGetTickCount ��ȡ��ʼʱ��
    TickType_t elapsed_time = 0;

    uint8_t detected_table[MAX_COURSE_NUM][CARD_ID_LENGTH] = {0};
    uint8_t detected_count = 0;

    uint8_t retry_count = 0;
    const uint8_t max_retries = 3; // ������Դ���
	uint8_t found_flag = 0;
    while(elapsed_time < pdMS_TO_TICKS(10000))
	{
        taskENTER_CRITICAL(); // �����ٽ�������ֹ����������
        // �������Ի���
        for(retry_count = 0; retry_count < max_retries; retry_count++)
		{
            if(USART0_GetRxFlag())
			{
                int found = 0;
                // ����Ƿ��Ѽ�⵽�ñ�ǩ
                for (uint8_t i = 0; i < detected_count; i++) {
                    if (memcmp(detected_table[i], USART0_RxPacket, CARD_ID_LENGTH) == 0) {
                        found = 1;
                        break;
                    }
                }

                // ������±�ǩ��δ�ﵽ������������¼
                if (!found && detected_count < MAX_COURSE_NUM) {
                    memcpy(detected_table[detected_count], USART0_RxPacket, CARD_ID_LENGTH);
                    detected_count++;
                    retry_count = 0; // �������Լ�����
                }
            }
            taskEXIT_CRITICAL(); // �˳��ٽ���
            vTaskDelay(pdMS_TO_TICKS(10)); // ��Ӷ��ӳ٣����ٳ�ͻ
            taskENTER_CRITICAL(); // ���½����ٽ���
        }
        taskEXIT_CRITICAL(); // �˳��ٽ���
        elapsed_time = xTaskGetTickCount() - start_time;
    }

    taskENTER_CRITICAL(); // �����ٽ�������ֹ����������
    
	
	//ʹ��USART0_SendString��ӡȱʧ�α�
    USART0_SendString("ȱʧ�α���\n");
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
			UART1_SendString("87");//ȱ�ٿα�����
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
			
			UART1_SendString("85");//ȱ�ٿα�����
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
	USART0_SendString("������\n");//��������֪ͨ
	
//	UART1_SendString("ȱʧ�α���\n");
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
//	UART1_SendString("������\n");
//	
	HMI_send_number("course_table.b2.bco",65535);
	HMI_send_number("course_table.b2.bco2",65535);
	HMI_send_string("course_table.b2.txt","���α�");
	
	detect_flag=0;	//��ʾ������
    taskEXIT_CRITICAL(); // �˳��ٽ���
}


