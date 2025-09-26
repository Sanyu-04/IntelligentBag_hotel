//-----------------------------------------------------------------------------

#include "4G.h"


u16 Num=0;
char Serial_RxPacket[100];//���յ�������
u8 dat[100];//��ȡ�·��ı�־�����ж��ŵ�ת��
uint8_t Serial_RxFlag;	//����������ݰ���־λ
u8 bize=0;
u8 ask[100]="config,get,lbsloc\r\n";//ѯ�ʵ�������
u8 rx_done=0;//��һ����ȡGPS
u8 size=0;

// ����ת��
u8 rx_data[400];//��������
u8 F_data[20]="config,set,sms,";//����ǰ������
u8 infor[256];//���ʱ��͵���λ��
char week[20]="Mon";//���ʱ��--���ڼ�
u8 tel[20]="13826683285";//��ŵ绰����

//�Ⱦ���
unsigned char D_key1[100]="e68891e98187e588b0e58db1e999a9e4ba86efbc8ce5bfabe69da5e69591e69591e68891efbc8c";
//������
unsigned char D_key[100]="e69da5e68ea5e68891efbc8c";
//Խ�紥��
unsigned char P_key[100]="e682a8e79a84e5ada9e5ad90e5b7b2e8b685e587bae5ae89e585a8e88c83e59bb4efbc8c";

//-------------------------------
u8 key_date[100]="{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"key\":{\"value\":true}}}";
u8 key_date1[100]="{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"key\":{\"value\":false}}}";

//���ڷ����ַ���
void Usart_SendStr( uint8_t *str)
{
	uint8_t i;
	for(i = 0;str[i] != '\0';i++)
	{
		UART0_SendByte(str[i]);
	}
}





//json��ϴ��룬�������json�����ϴ���ƽ̨
uint8_t Jansson2_Demo(int H,int tem)
{
	
    json_t *root;
    json_t *params_obj;
	json_t *temperature_obj;
	json_t *Humidity_obj;
	json_t *GPS_obj;
	json_t *day_obj;
    char *str;
	
	root = json_object();
	
    params_obj = json_object();
	temperature_obj = json_object();
	Humidity_obj = json_object();
	GPS_obj = json_object();
	day_obj = json_object();
	
    json_object_set_new(Humidity_obj, "value", json_integer(H)); // ����ʪ��Ƕ�׶��� { "value": H }
    json_object_set_new(temperature_obj, "value", json_integer(tem)); // �����¶�Ƕ�׶��� { "value": tem }
	
	json_object_set_new(GPS_obj, "value", json_string(Serial_RxPacket));
	json_object_set_new(day_obj, "value", json_string(week));
	
	// ��Ƕ�׶������params
    json_object_set_new(params_obj, "Humidity", Humidity_obj);
    json_object_set_new(params_obj, "temperature", temperature_obj);
    json_object_set_new(params_obj, "GPS", GPS_obj);
	json_object_set_new(params_obj, "day", day_obj);

    json_object_set_new(root, "id", json_string("123"));
    json_object_set_new(root, "version", json_string("1.0"));
    json_object_set_new(root, "params", params_obj);

    // ��� JSON_COMPACT ��־�Ƴ��ո�
    str = json_dumps(root, JSON_PRESERVE_ORDER | JSON_COMPACT);
    json_decref(root);
//	printf("=====================");
	printf("%s\r\n",str);
	//���ڷ��ͺ���
	//printf("str:%s\r\n",str);
    Usart_SendStr(str);
    free(str);  // ע�⣺json_dumps ������ڴ���Ҫ�� free() �ͷ�
    return 0;
}

//�������ϴ���
void all_key(unsigned char* R_data, unsigned char* tel, unsigned char* gg,unsigned char* oo, unsigned char* data)
{
    int i = 0;
    int i1 = 0;
    int i2 = 0;
    int i3=0;

    // ����data��R_data
    while (data[i] != '\0')
    {
        R_data[i] = data[i];
        i++;
    }
    // ����tel��R_data
    while (i1 != 11)
    {
        R_data[i + i1] = tel[i1];
        i1++;
    }

    R_data[26] =',';

    while (oo[i2] != '\0')
    {
        R_data[27+i2] = oo[i2];
        i2++;
    }

    // ����gg��R_data
    while (gg[i3] != '-')
    {
        R_data[27+i2+i3] = gg[i3];
        i3++;
    }
    R_data[i+i3 + i1 + i2+1] = '\r';
    R_data[i +i3+i1 + i2 + 2] = '\n';

    // ȷ��R_data�Կ��ַ���β
    R_data[i + i1 + i2+i3 + 3] = '\0';
}



//�������ϴ���
void all(u8* R_data, u8* tel, u8* gg, u8* data)
{
    int i = 0;
    int i1 = 0;
    int i2 = 0;

    // ����data��R_data
    while (data[i] != '\0')
    {
        R_data[i] = data[i];
        i++;
    }
    // ����tel��R_data
    while (i1 != 11)
    {
        R_data[i + i1] = tel[i1];
        i1++;
    }
    R_data[26] =',';
    // ����gg��R_data
    while (gg[i2] != '-')
    {
        R_data[27+i2] = gg[i2];
        i2++;
    }
    R_data[i + i1 + i2+1] = '\r';
    R_data[i + i1 + i2 + 2] = '\n';

    // ȷ��R_data�Կ��ַ���β
    R_data[i + i1 + i2 + 3] = '\0';
}


//time�Ľ��պ���
void extract_time_string(uint8_t* json, uint8_t* week_output,u8* infor)
{
	u8* i=json;
	int d=0;
	int op=0;
	int j1=0;
	int j2=0;
	int a=0;
	while(i[d]!='}')
	{
		if(i[d]=='-')
		{
			op+=1;
		}
		
		if(op==2&&a==3)
		{
			infor[j2]=i[d+1];
			j2++;
		}
		if(op==1&&a!=3)
		{
			week_output[j1]=i[d+1];
			a++;
			j1++;
		}
		d++;
	}
}



void Tel_Get(u8* dap)//�绰���պ���
{
	int i=0;
	int a=0;
	int k=0;
	int op=0;
	while(dap[i]!='}')
	{
		if(dap[i]=='+')
		{
			op=1;
		}
		if(op==1&&a!=11)
		{
			tel[k]=dap[i+1];
			a++;
			k++;
		}
		i++;
		//Usart_SendStr("ddd2");
	}
}
//�жϺ������жϵ绰�Ƿ�'22+'��־λ
int check_string_for_22(u8*data) {
    return (strstr((char*)data, "22+") != NULL);
}
u8 key_ch;
void Duan_xin(void) 
{
	//Usart_SendStr(dat);
	if(*dat!=NULL){
		printf("%s\r\n",dat);
		if(check_string_for_22(dat)==1) 
		{
			//���е绰����Ľ���
			Tel_Get(dat);
			printf("tel:%s\r\n",tel);
		}
		else
		{
			if(size==1)//��־λ���ж��ǽ��������������Ǳ�����Ϊ1������Ϊ0����
			{
				printf("key_ch:%c-----------\n",key_ch);
				extract_time_string(dat,week,infor);
				//���е绰����Ϣ�����ϣ��������������
				if(key_ch=='a')//�ж�����һ�����������ˣ���1�Ļ����·��������ҡ�
				{
					printf("key_ch=1-------\n");
					all_key(rx_data,tel,infor,D_key,F_data);
					printf("laijiew:%s",rx_data);
				}
				else if(key_ch=='b')//Ϊ0���·����Ⱦ��ҡ�
				{
					all_key(rx_data,tel,infor,D_key1,F_data);
					printf("qiuqiuw:%s",rx_data);
				}
//				printf("׼������\n");
				//���Ͷ���
				Usart_SendStr(rx_data);
//				Delay_ms(200);
				vTaskDelay(200);
				//��λkey
				Usart_SendStr(key_date1);//�ϴ�keyֵ��Ϊfalse
			}
			//����ʱ��Ͷ�������
			else
			{
				extract_time_string(dat,week,infor);
				//���е绰����Ϣ�����ϣ��������������
				//Usart_SendStr(week);
				//Delay_ms(10);
				//Usart_SendStr("\r\n");
				all_key(rx_data,tel,infor,P_key,F_data);
				printf("%s",rx_data);
				//���Ͷ���
				Usart_SendStr(rx_data);
			}
			size=0;
			//�ÿ�����
			*rx_data=NULL;
			*infor=NULL;
			*dat=NULL;
		}
	}
	HMI_send_string("course_table.week1.txt",week);
}
//---------------------------------





//u8 text[256]="config,set,sms,13826683285,3230e697b63137e588862ce6b5b7e78fa0e58cbae6bba8e6b19fe8a197e98193e8bf9ce5ae89e8b7af3738e58fb7\r\n";
//��������
void key_infor(void)
{
	
//	Delay_s(3);//��ʱ3s����С�����⵽key�ı仯
	//Usart_SendStr(text);//���ﵽʱ���·����Զ�����
	size=1;
	
}
u8 key_number;
void Find_key(void)
{
	//printf("jjjjj--------------");
	
	if(key_number=='a')
	{
		Usart_SendStr(key_date);//�ϴ�keyֵ��Ϊtrue
		key_ch='a';	//1�Ļ����·��������ҡ�
		key_infor();
	}
	else if(key_number=='b')
	{
		Usart_SendStr(key_date);//�ϴ�keyֵ��ΪTrue
//		Usart_SendStr(key_date1);//�ϴ�keyֵ��Ϊfalse
		key_infor();
		key_ch='b';	//�Ⱦ���
	}
	key_number=0;
}
void test_4G(void)
{
	int dd=24;
//	while(1)
//	{
	
		if(rx_done==1)
		{
			
			Jansson2_Demo(dd,dd);		
//			printf("-------------------------------------");
			vTaskDelay(100);
//			Delay_ms(100);
			Duan_xin();
			rx_done=0;
			
		}
		else
		{
			vTaskDelay(100);
//			Delay_ms(100);
			
			Usart_SendStr(ask);
//			printf("%s\r\n",ask);
			vTaskDelay(1000);
//			Delay_ms(1000);
			
		}
		
//	}
}
void UART0_IRQHandler(void)
{
	
	static u8 hh1=0;
	static u8 hh3=0;
	static u8 hh2=0;
    static uint8_t RxState1 = 0; // �����ʾ��ǰ״̬��״̬
    static uint8_t pRxPacket1 = 0; // �����ʾ��ǰ��������λ��
	static uint8_t RxState2 = 0; // �����ʾ��ǰ״̬��״̬
    static uint8_t pRxPacket2 = 0; // �����ʾ��ǰ��������λ��
		if(USART_GetFlagStatus(HT_UART0,USART_FLAG_RXDR)==SET)
		{
			uint8_t RxData = USART_ReceiveData(HT_UART0);
			if(RxData=='\r')
			{
				RxState1=1;
				hh1=0;
				RxState1=0;
				pRxPacket1=0;
				rx_done=1;
				hh3=0;
				printf("aaaaa");
				pRxPacket2=0;
				hh2=0;
				
			}
			
			if(RxData=='k')
			{
				hh3=1;
			}
			if(RxData=='1')
			{
				hh1=1;
			}
			if(RxData=='p'&&hh1==1)
			{
				hh2=1;
			}
			if(RxState2==0&&hh2==1)
			{
				dat[pRxPacket2]=RxData;
				pRxPacket2++;
			}
			if(RxState1==0&&hh1==1&&hh3==1)
			{
				Serial_RxPacket[pRxPacket1]=RxData;
				pRxPacket1++;
			}
			USART_ClearFlag(HT_UART0, USART_FLAG_RXDR);
		}
}

//�����ж�
//void USART1_IRQHandler(void)
//{
//    if(USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDNE) == SET)
//    {
//        uint8_t RxData = USART_ReceiveData(HT_USART1);
//		if(RxData==0x33)
//		{
//			key_number='a';
//		}
//		else if(RxData==0x44)
//		{
//			key_number='b';
//		}
//		//printf("%c",key_number);
//		Find_key();
//        
//    }
//}
