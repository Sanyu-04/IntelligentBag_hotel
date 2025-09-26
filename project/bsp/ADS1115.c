#include "ADS1115.h"
#include "USART1.h"
#include "usartHMI.h"
 
#define IIC_SCL(x)      ((x) ? GPIO_SetOutBits(HT_GPIOB,GPIO_PIN_0) : GPIO_ClearOutBits(HT_GPIOB,GPIO_PIN_0))
#define IIC_SDA(x)      ((x) ? GPIO_SetOutBits(HT_GPIOB,GPIO_PIN_1) : GPIO_ClearOutBits(HT_GPIOB,GPIO_PIN_1)) //SDA	 
#define READ_SDA   		GPIO_ReadInBit(HT_GPIOB,GPIO_PIN_1)  //????SDA 
						
void SDA_IN(){
  GPIO_DirectionConfig    (IN_PB1_GPIO_PORT, IN_PB1_GPIO_PIN, GPIO_DIR_IN);
  GPIO_InputConfig        (IN_PB1_GPIO_PORT, IN_PB1_GPIO_PIN, ENABLE);
  GPIO_PullResistorConfig (IN_PB1_GPIO_PORT, IN_PB1_GPIO_PIN, GPIO_PR_UP);
}

void SDA_OUT(){
  GPIO_SetOutBits         (OUT_PB1_GPIO_PORT, OUT_PB1_GPIO_PIN);
  GPIO_DirectionConfig    (OUT_PB1_GPIO_PORT, OUT_PB1_GPIO_PIN, GPIO_DIR_OUT);
  GPIO_PullResistorConfig (OUT_PB1_GPIO_PORT, OUT_PB1_GPIO_PIN, GPIO_PR_UP);
}

/******************************************************************
 * �� �� �� �ƣ�ADS1115_GPIO_Init
 * �� �� ˵ ������IIC���ų�ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC 
 * ��       ע��1100_0010_1000_0011  WriteADS1115(0x01,0xc2,0x83);
******************************************************************/
void ADS1115_GPIO_Init(void)
{                                
	SDA_OUT();		//SCL����Ĭ�Ͼ����������ģʽ
	IIC_SDA(1);
	IIC_SCL(1);
	
	//д�����ò���            
//	WriteADS1115(0x01,0xC2,0x83);  //4.096�������
	WriteADS1115(0x01,0xC0,0x83);

}

/******************************************************************
 * �� �� �� �ƣ�IIC_Start
 * �� �� ˵ ����IIC��ʼ�ź�
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void IIC_Start(void)
{
        SDA_OUT();
                   
        IIC_SDA(1);
//        ////Delay_us(5);
        IIC_SCL(1); 
//        ////Delay_us(5);
        
        IIC_SDA(0);
//        ////Delay_us(5);
        IIC_SCL(0);
//        ////Delay_us(5);
                       
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Stop
 * �� �� ˵ ����IICֹͣ�ź�
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void IIC_Stop(void)
{
        SDA_OUT();
        IIC_SCL(0);
        IIC_SDA(0);
        
        IIC_SCL(1);
//        ////Delay_us(5);
        IIC_SDA(1);
//        ////Delay_us(5);
        
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Send_Ack
 * �� �� ˵ ������������Ӧ��
 * �� �� �� �Σ�0Ӧ��  1��Ӧ��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void IIC_Send_Ack(unsigned char ack)
{
        SDA_OUT();
        IIC_SCL(0);
        IIC_SDA(0);
//        ////Delay_us(5);
        if(!ack) IIC_SDA(0);
        else     IIC_SDA(1);
        IIC_SCL(1);
//        ////Delay_us(5);
        IIC_SCL(0);
        IIC_SDA(1);
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Wait_Ack
 * �� �� ˵ �����ȴ��ӻ�Ӧ��
 * �� �� �� �Σ���
 * �� �� �� �أ�1=��Ӧ��   0=��Ӧ��
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/

unsigned char IIC_Wait_Ack(void)
{
        
        char ack = 0;
        unsigned char ack_flag = 10;
        SDA_IN();
        IIC_SDA(1);
//        ////Delay_us(5);
        IIC_SCL(1);
//        ////Delay_us(5);
        while( (READ_SDA==1) && ( ack_flag ) )
        {
                ack_flag--;
//                ////Delay_us(5);
        }
        
        if( ack_flag <= 0 )
        {
                IIC_Stop();
                return 1;
        }
        else
        {
                IIC_SCL(0);
                SDA_OUT();
        }
        return ack;
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Write
 * �� �� ˵ ����IICдһ���ֽ�
 * �� �� �� �Σ�datд�������
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void IIC_Write(unsigned char dat)
{
    int i = 0;
    SDA_OUT();
    IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    
    for( i = 0; i < 8; i++ )
    {
        IIC_SDA( (dat & 0x80) >> 7 );
//        ////Delay_us(2);
        dat<<=1;
//        ////Delay_us(6); 
        IIC_SCL(1);
//        ////Delay_us(4);
        IIC_SCL(0);
//        ////Delay_us(4);
            
    }        
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Read
 * �� �� ˵ ����IIC��1���ֽ�
 * �� �� �� �Σ���
 * �� �� �� �أ�������1���ֽ�����
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
unsigned char IIC_Read(void)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
    {
        IIC_SCL(0);
//        ////Delay_us(5);
        IIC_SCL(1);
//        ////Delay_us(5);
        receive<<=1;
        if( READ_SDA )
        {        
            receive|=1;   
        }
        ////Delay_us(5); 
    }                
  return receive;
}
/******************************************************************
 * �� �� �� �ƣ�WriteADS1115
 * �� �� ˵ ������ADS1115��add��ַд��dat����
 * �� �� �� �Σ�addд��Ĵ�����ַ dat_Hд��ĸ�8λ����  dat_Lд��ĵ�8λ����
 * �� �� �� �أ�0д��ɹ� 1д��������ַ��Ӧ��  2д��Ĵ�����ַ��Ӧ�� 
 *              3д���8λ������Ӧ��  4д���8λ������Ӧ��
 * ��       �ߣ�LC
 * ��       ע��������ַ=0X90   
******************************************************************/
uint8_t WriteADS1115(uint8_t add,uint8_t dat_H,uint8_t dat_L)
{
	IIC_Start();
	IIC_Write(0x90);     
	if( IIC_Wait_Ack() == 1 )
	{      
//		printf("error 1\r\n");
		return 1; 
	}
	IIC_Write(add);   
	if( IIC_Wait_Ack() == 1 ) 
	{
//		printf("error 2\r\n");
		return 2;     
	}
	IIC_Write(dat_H);     
	IIC_Wait_Ack();
	IIC_Write(dat_L);     
	IIC_Wait_Ack();
	IIC_Stop();  
          return (0);
}
/******************************************************************
 * �� �� �� �ƣ�ReadADS1115
 * �� �� ˵ ������ȡADS1115������
 * �� �� �� �Σ�add��ȡ�ļĴ�����ַ
 * �� �� �� �أ�-1-��ȡʧ��  ����-��ȡ�ɹ�
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
float ReadADS1115(unsigned char add)
{
	int i =0;
	unsigned char dat[2]={0};
	unsigned int num = 0;
	float ret=0;
	IIC_Start();//��ʼ�ź�
	IIC_Write(0x90);//������ַ+д
	if( IIC_Wait_Ack() == 1 ) 
		return -1;
	IIC_Write(add);//�Ĵ�����ַ
	if( IIC_Wait_Ack() == 1 ) 
		return -1;  
	do{
	//��ʱ�ж�
	i++;
	if( i > 20 ) return -1;  
	//Delay_ms(1);

		
	IIC_Start();//���·�����ʼ�ź�
	IIC_Write(0x91);//������ַ+��
	}while(IIC_Wait_Ack() == 1);
	
	dat[0]=IIC_Read();//����8λ���� 
	IIC_Send_Ack(0);//Ӧ��
	dat[1]=IIC_Read();//����8λ���� 
	IIC_Send_Ack(1);//��Ӧ��  
	IIC_Stop();//����ֹͣ�ź�
	//��������
	num =  ((dat[0]<<8) | (dat[1])); 

        //��ֵ����ȡ����PGA����
    //2��15�η�=32768
    //���õ��������4.096
//        if(num>32768)
//                ret=((float)(65535-num)/32768.0)*4.096;
//        else
//                ret=((float)num/32768.0)*4.096;

  //�ֱ��ʼ��㣺������ѹ��Χ/(2^ADλ��-1)
  //    �ֱ���= 4.096/2^15=0.000125
  //      ��ѹ= �ɼ�����ADCֵ * �ֱ���
  
	//���ڽ�������̸���Ϊ6.144V
	if(num>32768)
		ret=(65535-num)*0.000125;
//	ret=(65535-num)*0.0001875;
	else
		ret=num*0.0001875;
	
	return ret;
}

void battery_adc(void)
{
	double battery_voltage=0;	//������ʵʱ��ѹ

	battery_voltage=ReadADS1115(0x00);
	int battery_percentage=(battery_voltage - 4.4)/1.08 * 100;
//	printf("����:%d\r\n",battery_percentage);
//	printf("��ѹ:%.4f\r\n",battery_voltage);
	
	if(battery_percentage > 100){
		HMI_send_number("index.n0.val",100);
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
	}
	else if(battery_percentage < 0 ){
		HMI_send_number("index.n0.val",0);
	}
	else{
	HMI_send_number("index.n0.val",battery_percentage);
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
		USART0_SendString("111111111111\n");
	}
}


