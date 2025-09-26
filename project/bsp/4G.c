//-----------------------------------------------------------------------------

#include "4G.h"


u16 Num=0;
char Serial_RxPacket[100];//接收地理坐标
u8 dat[100];//获取下发的标志，进行短信的转发
uint8_t Serial_RxFlag;	//定义接收数据包标志位
u8 bize=0;
u8 ask[100]="config,get,lbsloc\r\n";//询问地理坐标
u8 rx_done=0;//第一次收取GPS
u8 size=0;

// 短信转发
u8 rx_data[400];//短信数组
u8 F_data[20]="config,set,sms,";//短信前置数组
u8 infor[256];//存放时间和地理位置
char week[20]="Mon";//存放时间--星期几
u8 tel[20]="13826683285";//存放电话号码

//救救我
unsigned char D_key1[100]="e68891e98187e588b0e58db1e999a9e4ba86efbc8ce5bfabe69da5e69591e69591e68891efbc8c";
//来接我
unsigned char D_key[100]="e69da5e68ea5e68891efbc8c";
//越界触发
unsigned char P_key[100]="e682a8e79a84e5ada9e5ad90e5b7b2e8b685e587bae5ae89e585a8e88c83e59bb4efbc8c";

//-------------------------------
u8 key_date[100]="{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"key\":{\"value\":true}}}";
u8 key_date1[100]="{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"key\":{\"value\":false}}}";

//串口发送字符串
void Usart_SendStr( uint8_t *str)
{
	uint8_t i;
	for(i = 0;str[i] != '\0';i++)
	{
		UART0_SendByte(str[i]);
	}
}





//json组合代码，将所需的json数据上传云平台
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
	
    json_object_set_new(Humidity_obj, "value", json_integer(H)); // 创建湿度嵌套对象 { "value": H }
    json_object_set_new(temperature_obj, "value", json_integer(tem)); // 创建温度嵌套对象 { "value": tem }
	
	json_object_set_new(GPS_obj, "value", json_string(Serial_RxPacket));
	json_object_set_new(day_obj, "value", json_string(week));
	
	// 将嵌套对象加入params
    json_object_set_new(params_obj, "Humidity", Humidity_obj);
    json_object_set_new(params_obj, "temperature", temperature_obj);
    json_object_set_new(params_obj, "GPS", GPS_obj);
	json_object_set_new(params_obj, "day", day_obj);

    json_object_set_new(root, "id", json_string("123"));
    json_object_set_new(root, "version", json_string("1.0"));
    json_object_set_new(root, "params", params_obj);

    // 添加 JSON_COMPACT 标志移除空格
    str = json_dumps(root, JSON_PRESERVE_ORDER | JSON_COMPACT);
    json_decref(root);
//	printf("=====================");
	printf("%s\r\n",str);
	//串口发送函数
	//printf("str:%s\r\n",str);
    Usart_SendStr(str);
    free(str);  // 注意：json_dumps 分配的内存需要用 free() 释放
    return 0;
}

//短信整合代码
void all_key(unsigned char* R_data, unsigned char* tel, unsigned char* gg,unsigned char* oo, unsigned char* data)
{
    int i = 0;
    int i1 = 0;
    int i2 = 0;
    int i3=0;

    // 复制data到R_data
    while (data[i] != '\0')
    {
        R_data[i] = data[i];
        i++;
    }
    // 复制tel到R_data
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

    // 复制gg到R_data
    while (gg[i3] != '-')
    {
        R_data[27+i2+i3] = gg[i3];
        i3++;
    }
    R_data[i+i3 + i1 + i2+1] = '\r';
    R_data[i +i3+i1 + i2 + 2] = '\n';

    // 确保R_data以空字符结尾
    R_data[i + i1 + i2+i3 + 3] = '\0';
}



//短信整合代码
void all(u8* R_data, u8* tel, u8* gg, u8* data)
{
    int i = 0;
    int i1 = 0;
    int i2 = 0;

    // 复制data到R_data
    while (data[i] != '\0')
    {
        R_data[i] = data[i];
        i++;
    }
    // 复制tel到R_data
    while (i1 != 11)
    {
        R_data[i + i1] = tel[i1];
        i1++;
    }
    R_data[26] =',';
    // 复制gg到R_data
    while (gg[i2] != '-')
    {
        R_data[27+i2] = gg[i2];
        i2++;
    }
    R_data[i + i1 + i2+1] = '\r';
    R_data[i + i1 + i2 + 2] = '\n';

    // 确保R_data以空字符结尾
    R_data[i + i1 + i2 + 3] = '\0';
}


//time的接收函数
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



void Tel_Get(u8* dap)//电话接收函数
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
//判断函数，判断电话是否含'22+'标志位
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
			//进行电话号码的接收
			Tel_Get(dat);
			printf("tel:%s\r\n",tel);
		}
		else
		{
			if(size==1)//标志位，判断是进行主动触发还是被动，为1主动，为0被动
			{
				printf("key_ch:%c-----------\n",key_ch);
				extract_time_string(dat,week,infor);
				//进行电话，信息的整合，加入短信内容内
				if(key_ch=='a')//判断是哪一个按键按下了，是1的话就下发“来接我”
				{
					printf("key_ch=1-------\n");
					all_key(rx_data,tel,infor,D_key,F_data);
					printf("laijiew:%s",rx_data);
				}
				else if(key_ch=='b')//为0，下发“救救我”
				{
					all_key(rx_data,tel,infor,D_key1,F_data);
					printf("qiuqiuw:%s",rx_data);
				}
//				printf("准备发送\n");
				//发送短信
				Usart_SendStr(rx_data);
//				Delay_ms(200);
				vTaskDelay(200);
				//复位key
				Usart_SendStr(key_date1);//上传key值置为false
			}
			//接收时间和短信内容
			else
			{
				extract_time_string(dat,week,infor);
				//进行电话，信息的整合，加入短信内容内
				//Usart_SendStr(week);
				//Delay_ms(10);
				//Usart_SendStr("\r\n");
				all_key(rx_data,tel,infor,P_key,F_data);
				printf("%s",rx_data);
				//发送短信
				Usart_SendStr(rx_data);
			}
			size=0;
			//置空数组
			*rx_data=NULL;
			*infor=NULL;
			*dat=NULL;
		}
	}
	HMI_send_string("course_table.week1.txt",week);
}
//---------------------------------





//u8 text[256]="config,set,sms,13826683285,3230e697b63137e588862ce6b5b7e78fa0e58cbae6bba8e6b19fe8a197e98193e8bf9ce5ae89e8b7af3738e58fb7\r\n";
//按键短信
void key_infor(void)
{
	
//	Delay_s(3);//延时3s，让小程序检测到key的变化
	//Usart_SendStr(text);//这里到时候下发会自动发送
	size=1;
	
}
u8 key_number;
void Find_key(void)
{
	//printf("jjjjj--------------");
	
	if(key_number=='a')
	{
		Usart_SendStr(key_date);//上传key值置为true
		key_ch='a';	//1的话就下发“来接我”
		key_infor();
	}
	else if(key_number=='b')
	{
		Usart_SendStr(key_date);//上传key值置为True
//		Usart_SendStr(key_date1);//上传key值置为false
		key_infor();
		key_ch='b';	//救救我
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
    static uint8_t RxState1 = 0; // 定义表示当前状态机状态
    static uint8_t pRxPacket1 = 0; // 定义表示当前接收数据位置
	static uint8_t RxState2 = 0; // 定义表示当前状态机状态
    static uint8_t pRxPacket2 = 0; // 定义表示当前接收数据位置
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

//测试中断
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
