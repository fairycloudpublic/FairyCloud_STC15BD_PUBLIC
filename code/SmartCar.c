/* ----------------------------------------- ��Ҫ˵�� -----------------------------------------------------------------*/

/*--1.��������������github���������µ�-------------------------------------------------*/
/*--2.P2^5��LED̨��----------------------------------------------------------------------------------------------------*/
/*--3.P4^1��Buzzer�澯-------------------------------------------------------------------------------------------------*/
/*--4.OLED��ʾ�������Ŷ�Ӧ��ϵ  SCL=P2^1; ����ʱ��   SDA=P2^0; ��������---��ͷ�ļ�OLED.H�� 13 14���޸�----------------------*/
/* ------------------------------Զ�����΢��С���򡶾�����������--------------------------------------------------------------------------*/


/* ------------------------------��ӭ��������--------------------------------------------------------------------------*/
/* ---------------------------- QQȺ:630017549-------------------------------------------------------------------------*/
/* -----------------------------΢��: fairycloud2035-----------------------------------------------------------------------*/
/* -------------------------------QQ:1055417926------------------------------------------------------------------------*/

/* ------------------------------------ END ---------------------------------------------------------------------------*/
 

#include "STC15W4K58S4.h"
#include "DHT11.h"
#include "intrins.h"
#include "stdio.h"
#include "OLED/oled.h"

#include "config.h"
#include "delay.h"

//�ƺͰ�ť����
sbit Buzzer = P4^1;  // Buzzer������
sbit LED = P2^5;  // LED̨��
sbit LED2 = P2^6;
sbit LED3 = P2^7;
sbit SW1 = P5^3;
sbit SW2 = P0^5;
sbit SW3 = P0^6;

u8 xdata RX1_Buffer[450];	//���ջ���
u8	RX1_Cnt;	//���ռ���
u8 mqtt_message[256];//MQTT���ͻ���

bit busy;

typedef char I8;
typedef int I16;
typedef long I32;
typedef unsigned char U8; 

//��¼�豸������״̬
static unsigned char DeviceStatus = 0;


static	  unsigned char   AutoSendMsgFlag = 0;// flag

unsigned int TEMP = 0; 
unsigned int HUMI = 0; 

static	 unsigned int   Timer4_Count=1;
static	 unsigned int   Timeout_Count=0;
static	 unsigned int   DisplayTime_Count=0;
static   unsigned char i;

#define FOSC 11059200L          //ϵͳƵ��
#define BAUD 115200             //���ڲ�����

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


void DELAY_MS(unsigned int timeout);		//@11.0592MHz   1ms
void DELAY_1MS() ;
void UART_TC (unsigned char *str);//���������UART_T����
void UART_T (unsigned char UART_data); //���崮�ڷ������ݱ���
void UART_R();//��������
void ReConnectServer();//����WIFI���ӷ�����
void USART1_Init();
void Device_Init();
void Timer4Init();
void Buzzer_Actions_Status(unsigned char status);
void Led_Actions_Status(unsigned char status);

void Timer0Init(void);
void OLEDFunc(unsigned char TEMP,unsigned char HUMI);
void AutoSendMsg();
void CloudHandleReceive(void);

void ESP8266_SmartConnect();
void Enter_ErrorMode(U8 mode);



void main(){
		P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x20;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;
		
		Device_Init();//��ʼ��Ӳ��


		OLED_Init();
	  OLED_Clear(0);

		OLEDFunc(0,0);
			OLED_ShowString(0,0,"0000/00/00 00:00",16);

		OLED_ShowString(0,7,"Starting        ",8);//connected closed starting


    USART1_Init();//��ʼ����WiFiͨ�ŵĴ���

		if(PCON&0x10){	//�����Ӳ����(�ϵ�����)�Ļ����ͽ���WiFi�ĵ�һ�γ�ʼ����������������������λ�������Ź��������Ļ�ֱ����������ΪWiFi�ڵ�һ�γ�ʼ����ʱ�򣬾ͽ����ˡ� ����TCP���ӵ�flash��ʵ���ϵ�͸����
			PCON&=0xef;
			ReConnectServer();
			ESP8266_SmartConnect();
		}

				
		Timer4Init();
		Timer0Init();

	  WDT_CONTR = 0x06;       //���Ź���ʱ�����ʱ����㹫ʽ: (12 * 32768 * PS) / FOSC (��)
                            //���ÿ��Ź���ʱ����Ƶ��Ϊ32,���ʱ������:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //�������Ź�  STC��Ƭ���Ŀ��Ź�һ�������󣬾�û���ر�

    while(1) {
			WDT_CONTR |= 0x10;  //ι������
			
			if(DHT11_Read_Data(&DATA_Temphui[0],&DATA_Temphui[1])==0)//��ʪ�ȼ��
			{
				
				  DATA_Temphui[2]=1;	 
				  TEMP = DATA_Temphui[0];
				  HUMI = DATA_Temphui[1];
					OLEDFunc(DATA_Temphui[0],DATA_Temphui[1]);
			}
			
			if(AutoSendMsgFlag == 1)
			{
				AutoSendMsgFlag=0;
				AutoSendMsg( );

      }
    }
}

//OLED��Ļ��ʾ
void OLEDFunc(unsigned char TEMP,unsigned char HUMI){
			//OLED_ShowString(0,0,"0000/00/00 00:00",16);
	
	//��ʪ����ʾ
		OLED_ShowString(0,2,"Temp:",16);
		OLED_ShowString(72,2,"Humi:",16);
		
	  OLED_ShowChar(40,2,'0'+TEMP/10%10,16);
	  OLED_ShowChar(48,2,'0'+TEMP%10,16);		
		OLED_ShowChar(112,2,'0'+HUMI/10%10,16);
	  OLED_ShowChar(120,2,'0'+HUMI%10,16);


	
		 OLED_ShowCHinese(0,4,0);//̨��
		 OLED_ShowCHinese(16,4,1);

		 OLED_ShowCHinese(72,4,2);//�澯
		 OLED_ShowCHinese(88,4,3);


		 OLED_ShowChar(32,4,':',16);
		 OLED_ShowChar(104,4,':',16);	

		if(!LED){//��ʾ̨��״̬ ��/��
				OLED_ShowCHinese(40,4,4);
		}else{
				OLED_ShowCHinese(40,4,5);
		}
		
		if(!Buzzer){//��ʾ�澯״̬ ��/��
				OLED_ShowCHinese(112,4,4);
		}else{
				OLED_ShowCHinese(112,4,5);
		}

//�쳣״̬��ʾ
		if(DeviceStatus==0){
				OLED_ShowString(0,7,"OK              ",8);
		}else{
			Enter_ErrorMode(DeviceStatus);
		}
		

}


void AutoSendMsg(){
	
	   unsigned char *strLED ;
	   unsigned char *strBuzzer ;
	
		 //ˢ��һ��LED��Ļ
		OLED_Init();
		OLED_Clear(0);
	
		if(!LED){//��ʾ̨��״̬ ��/��
				strLED = "open";
		}else{
				strLED = "close";
		}
		
		if(!Buzzer){//��ʾ�澯״̬ ��/��
				strBuzzer = "open";
		}else{
				strBuzzer = "close";
		}
		
		


			//ע�����ݳ��Ȳ�Ҫ����mqtt_message��256 Ŀǰ��219
			sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"cmdtype\\\":\\\"cmd_status\\\"\\\,\\\"datatype\\\":\\\"dictionary\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"temperature\\\":%d\\\,\\\"humidity\\\":%d\\\,\\\"led\\\":\\\"%s\\\"\\\,\\\"alarm\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"\}\",0,0\r\n\0",DPubTopic,DCID,TEMP,HUMI,strLED,strBuzzer,Dversion);
			UART_TC(mqtt_message);
			memset(mqtt_message,0,256);

			if(strstr(RX1_Buffer,"OK")!=NULL)
			{
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else 
			{
				Enter_ErrorMode(6);
			}		
}


void DELAY_1MS() {
    unsigned char i, j;

    _nop_();
    _nop_();
    _nop_();
    i = 11;
    j = 190;
    do
    {
        while (--j);
    } while (--i);


}

void DELAY_MS(unsigned int timeout)		//@11.0592MHz
{
    int t = 0;
    while (t < timeout)
    {
        t++;
        DELAY_1MS();
    }
}



//��ʼ��LED̨�ƺ�Buzzer�澯 //0�ǿ��� 1�ǹر�
void Device_Init() {

    LED = 1;
    Buzzer = 1;
}


void USART1_Init()
{

    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
    ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
    P_SW1 = ACC;
    SCON = 0x50;                //8λ�ɱ䲨����
    PS = 1;
		
	
    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    EA = 1;

}



/*----------------------------
UART �жϽ��շ������
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        while(!RI);
        RI=0;
        UART_R();
        busy = 0;

    }
    if (TI)
    {
        while(!TI);
        TI = 0;                 //���TIλ
        busy = 0;               //��æ��־
    }
}

//����  ����ESP8266�Ĵ������ݣ���У�����ݵ�������9λ

void UART_R()
{
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0xDC;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;	//�����ж�
	
	
	RX1_Buffer[RX1_Cnt] = SBUF;
	if(++RX1_Cnt >= 450)	RX1_Cnt = 0;

}


//���ڷ��������ַ���������\0ֹͣ

void UART_TC (unsigned char *str) {
	 EA = 0;
    while(*str != '\0') {
        UART_T(*str);
        *str++;
    }
    *str = 0;
		 EA = 1;
}

void UART_T (unsigned char UART_data) { //���崮�ڷ������ݱ���
    SBUF = UART_data;	//�����յ����ݷ��ͻ�ȥ
    while(!TI);		//��鷢���жϱ�־λ
    TI = 0;			//����жϱ�־λΪ0��������㣩
}



//���� ESP8266WiFiģ��
void ReConnectServer() {

    UART_TC("AT+RST\r\n\0");  // ��λ
			
}


void Timer4Init(void)		
{
		//50 ����@11.0592MHz
		T4T3M &= 0xDF;		//��ʱ��ʱ��12Tģʽ
		T4L = 0x00;		//���ö�ʱ��ֵ
		T4H = 0x4C;		//���ö�ʱ��ֵ
		T4T3M |= 0x80;		//��ʱ��4��ʼ��ʱ
	
		IE2 |= 0x40;		//����ʱ��4�ж�
		EA=1; 	//���жϿ���
}

//10s�ж��Զ��ϱ���Ϣ
void Timer4_interrupt() interrupt 20    //��ʱ�ж����
	{
		
		if(DisplayTime_Count>=20){  //20 * 50ms = 1s
			DisplayTime_Count = 0;
			
			
		}else{
			 DisplayTime_Count++;//ÿ��һ�μ�50ms
		}
				
		if(Timer4_Count>=200){  //200 * 50ms = 10s
			AutoSendMsgFlag = 1;
			Timer4_Count = 0;

//				Timeout_Count++;//ÿ��һ�μ�10s
//				
//				if(Timeout_Count < 3 ){ //û����֤�ɹ����˷�����Ҳ��ܾ�
//					//  AutoSendMsgFlag = 1;

//				}else if(Timeout_Count >= 3){//30s�����ղ������������ص����ݣ��� ��������
//						//��������ƽ̨
//					//ReConnectServer();
//					Timeout_Count = 0;
//					OLED_ShowString(0,7,"Closed          ",8);//connected closed starting

//				}
						
		}else{
						
				Timer4_Count++;
		}
					

		
}






void Timer0Init(void)		//10����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0xB8;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;	//�����ж�
}



/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt 1
{
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0xB8;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0��ʼ��ʱ
	ET0 = 0;	//�����ж�
	
	CloudHandleReceive();
	RX1_Cnt = 0;
}


void Led_Actions_Status(unsigned char status){  //0�ǿ��� 1�ǹر�

	if(status){
		LED = 1;
	}else{
		LED = 0;
	}

}

void Buzzer_Actions_Status(unsigned char status){ //0�ǿ��� 1�ǹر�

	if(status){
		Buzzer = 1;
	}else{
		Buzzer = 0;
	}

}

/******************************  ����һ������ģʽ  *****************************/
void ESP8266_SmartConnect(void)
{
	
	u8 status= 1;
	DeviceStatus = 1;
	while(status==1)
	{
			OLED_ShowString(0,7,"AT              ",8);
			memset(RX1_Buffer,0,450);RX1_Cnt = 0;
			UART_TC("AT\r\n\0"); 
		  DELAY_MS(500);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=2;
				DeviceStatus = 2;

        RX1_Cnt = 0;
				memset(RX1_Buffer,0,450);
			}
			else
			{
				Enter_ErrorMode(1);
			}
	}	
	while(status==2)
	{
				OLED_ShowString(0,7,"CWMODE          ",8);

				UART_TC("AT+CWMODE=1\r\n\0");DELAY_MS(500);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				//PrintString1("\ģʽѡ��ɹ�\r\n" );
				//PrintString1(RX1_Buffer);
				RX1_Cnt = 0;
				memset(RX1_Buffer,0,450);
				status=3;
				DeviceStatus = 3;

			}
			else
			{
				status=2;
				Enter_ErrorMode(2);

			}
	}
	if(status==3)//����WiFiģ�鹤��ģʽΪ��STAģʽ���������ñ�����flash
	{
				OLED_ShowString(0,7,"CWJAP           ",8);
			sprintf(mqtt_message,"AT+CWJAP=\"%s\",\"%s\"\r\n\0",WiFiName,WiFiPassword);
			UART_TC(mqtt_message);
			memset(mqtt_message,0,256);
			DELAY_MS(4000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=4;
				DeviceStatus = 4;
				//UART_TC("\r���������ɹ�SUCCESS\r\n\0" );
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else
			{
				//UART_TC("\r��������FAIL\r\n\0" );
				status=1;
				Enter_ErrorMode(3);
			}
			
	}
	
	while(status==4)
	{
		
			OLED_ShowString(0,7,"MQTTUSERCFG     ",8);

			UART_TC("AT+MQTTUSERCFG=0,1,\"\",\"\",\"\",0,0,\"\"\r\n\0"); 
			memset(mqtt_message,0,256);
			DELAY_MS(2000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=5;
								DeviceStatus = 5;

				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else
			{
				status=1;
				Enter_ErrorMode(4);
			}
	}
	while(status==5)//����WiFiģ�鹤��ģʽΪ��STAģʽ���������ñ�����flash
	{
			OLED_ShowString(0,7,"MQTTUSERNAME    ",8);

			sprintf(mqtt_message,"AT+MQTTUSERNAME=0,\"%s\"\r\n\0",DName);
			UART_TC(mqtt_message);
			memset(mqtt_message,0,256);
			DELAY_MS(1000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=6;
				DeviceStatus = 6;
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}	
			else
			{
				status=1;
				Enter_ErrorMode(5);
			}
	}
	while(status==6)
	{
			OLED_ShowString(0,7,"MQTTPASSWORD    ",8);

		  sprintf(mqtt_message,"AT+MQTTPASSWORD=0,\"%s\"\r\n\0",DPassWord);
		  UART_TC(mqtt_message);
			memset(mqtt_message,0,256);
			DELAY_MS(1000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{
				status=7;
				DeviceStatus = 7;
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else 
			{	
				status=1;
				Enter_ErrorMode(6);
			}
	}	
	while(status==7)
	{
			OLED_ShowString(0,7,"MQTTCLIENTID    ",8);

			sprintf(mqtt_message,"AT+MQTTCLIENTID=0,\"%s\"\r\n\0",DClientID);
			UART_TC(mqtt_message);
			DELAY_MS(2000);
			memset(mqtt_message,0,256);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=8;
				DeviceStatus = 8;
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else 
			{			
				status=1;
				Enter_ErrorMode(7);
			}
	}
	while(status==8)
	{
			OLED_ShowString(0,7,"MQTTCONN        ",8);
			sprintf(mqtt_message,"AT+MQTTCONN=0,\"%s\",%d,1\r\n\0",mqttHostUrl,port);
			UART_TC(mqtt_message);
			DELAY_MS(2000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{	
				status=9;
				DeviceStatus = 9;
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else 
			{			
				status=1;
				Enter_ErrorMode(8);
			}
	}
	if(status==9)
	{
			OLED_ShowString(0,7,"MQTTSUB         ",8);
			sprintf(mqtt_message,"AT+MQTTSUB=0,\"%s\",0\r\n\0",DSubTopic);
			UART_TC(mqtt_message);
			memset(mqtt_message,0,256);
			//UART_TC("AT+MQTTSUB=0,\"/sub/\",1883,0\r\n\0");
			DELAY_MS(2000);
			if(strstr(RX1_Buffer,"OK")!=NULL)
			{
				DeviceStatus = 0;
				OLED_ShowString(0,7,"OK              ",8);
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else 
			{
				status=1;
				Enter_ErrorMode(9);
			}
			
		  AutoSendMsgFlag=1;
		

	}
}

/******************************  �������ģʽ����  *****************************/

//�������ģʽ�ȴ��ֶ�����
void Enter_ErrorMode(u8 mode)
{
		switch(mode){
			case 0:	
					OLED_ShowString(0,7,"OK              ",8);
					break;
			case 1:	
					OLED_ShowString(0,7,"E1 AT           ",8);
					break;
			case 2:
					OLED_ShowString(0,7,"E2 CWMODE       ",8);
					break;
			case 3:		
					OLED_ShowString(0,7,"E3 CWJAP        ",8);
					 ReConnectServer();DELAY_MS(1500);ESP8266_SmartConnect();
					break;
			case 4:	
					OLED_ShowString(0,7,"E4 MQTTUSERCFG  ",8);
					break;
			case 5:	
					OLED_ShowString(0,7,"E5 MQTTUSERNAME ",8);
					break;
			case 6:	
					OLED_ShowString(0,7,"E6 MQTTPASSWORD ",8);
					break;
			case 7:
					OLED_ShowString(0,7,"E7 MQTTCLIENTID ",8);
			    ReConnectServer();DELAY_MS(1500);ESP8266_SmartConnect();
					break;
			case 8:
			    OLED_ShowString(0,7,"E8 MQTTCONN     ",8);
			    ReConnectServer();DELAY_MS(1500);ESP8266_SmartConnect();
					break;
			case 9:
			    OLED_ShowString(0,7,"E9 MQTTSUB      ",8);
			    ReConnectServer();DELAY_MS(1500);ESP8266_SmartConnect();
					break;
			default:
					OLED_ShowString(0,7,"EE              ",8);
					ReConnectServer();DELAY_MS(1500);ESP8266_SmartConnect();

			break;
		}		
}

void CloudHandleReceive(void)
{
	

    if (strncmp(RX1_Buffer, "ERROR", 5) == 0) //����ִ���쳣
    {
        //Ŀǰ������,������
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
    }
    else if (strncmp(RX1_Buffer, "OK", 2) == 0) //����ִ������
    {
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
    }
    else if (RX1_Buffer[0] == '+') //��ȡ��ϸ��Ϣ
    {
			

						if (strncmp(RX1_Buffer + 1, "MQTTSUBRECV", 11) == 0) //�յ�������Ϣ
						{

								pdata U8 *P=RX1_Buffer;//�����￪ʼ���ҳ���
								pdata U8 name[20],i=0;
								pdata U8 reporttime[16];
							  memset(name,0,20);
							  memset(reporttime,0,16);
							  i=0;
					      
							
							//ƥ���һ�γ��ֵĵ�ַ  ״̬��ѯ
								P=strstr(P,"\"cmdtype\":\"cmd_status\"");
										
							  if(P != NULL){
												sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
													
													AutoSendMsgFlag = 1;

								}
							
							
								i=0;
								P=RX1_Buffer;

								
							 //ƥ���һ�γ��ֵĵ�ַ  ��ʾ���ص�ʱ���
								P=strstr(P,"\"reporttime\":\"");
										
							  if(P != NULL){
											P+=14;
                      while(*P != '"'&&i!=16)
												reporttime[i++]=*P++;										
			            OLED_ShowString(0,0,reporttime,16);

								}
							
							
								i=0;
								P=RX1_Buffer;

								//ƥ���һ�γ��ֵĵ�ַ  ƥ��Զ��ָ��
								P=strstr(P,"\"sensorname\":\"");
								
								//û��ƥ�䵽Զ��ָ��
							  if(P == NULL){

										memset(RX1_Buffer,0,450);
										RX1_Cnt = 0;
										memset(name,0,20);
										i=0;
									
								//ƥ�䵽Զ��ָ��
								}else{
									
											P+=14;
											//Զ��ָ������
											while(*P != '"'&&i!=19)
												name[i++]=*P++;
											name[i]=0;
										//״̬��ѯ
										if(strncmp(name,"status",6) == 0){
												memset(name,0,20);
												i=0;
												
												P=RX1_Buffer;
												P=strstr(P,"\"sensorcmd\":\""),P+=13;
												
												while(*P != '"'&&i!=19)
													name[i++]=*P++;
												name[i]=0;
												
												if(strncmp(name,"open",4) == 0){
													
												}else if(strncmp(name,"close",5) == 0){
												
												}else{}
												
													sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
													AutoSendMsgFlag = 1;
											}	
										//����
											else if(strncmp(name,"restart",7) == 0){
											
													sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
												
													IAP_CONTR = 0X20;
											}	
											//��
											else if(strncmp(name,"led",3) == 0){
												memset(name,0,20);
												i=0;
												
												P=RX1_Buffer;
												P=strstr(P,"\"sensorcmd\":\""),P+=13;
												
												while(*P != '"'&&i!=19)
													name[i++]=*P++;
												name[i]=0;
												
												if(strncmp(name,"open",4) == 0){
													Led_Actions_Status(0);
												}else if(strncmp(name,"close",5) == 0){
													Led_Actions_Status(1);
												}else{}
												
													sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
													AutoSendMsgFlag = 1;
											}	
											//�澯
											else if(strncmp(name,"alarm",5) == 0){

												memset(name,0,20);

												P=RX1_Buffer;
												P=strstr(P,"\"sensorcmd\":\""),P+=13;
												i=0;
												while(*P != '"'&&i!=19)
													name[i++]=*P++;
												name[i]=0;
												
												if(strncmp(name,"open",4) == 0){
													
													Buzzer_Actions_Status(0);

												}else if(strncmp(name,"close",5) == 0){
													Buzzer_Actions_Status(1);

												}else{
												
												}
											  sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                  UART_TC(mqtt_message);
												
												AutoSendMsgFlag = 1;


											}	
												
										
								
								
								
								
								}

							memset(RX1_Buffer,0,450);
							RX1_Cnt = 0;
							

								
						}
						
						
						RX1_Cnt = 0;

			
						
        
    }
    else //��ȡ������Ϣ
    {
        //Ŀǰ�ò���
    }
}


