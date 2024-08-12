/* ----------------------------------------- 重要说明 -----------------------------------------------------------------*/

/*--1.《精灵物联网》github代码是最新的-------------------------------------------------*/
/*--2.P2^5是LED台灯----------------------------------------------------------------------------------------------------*/
/*--3.P4^1是Buzzer告警-------------------------------------------------------------------------------------------------*/
/*--4.OLED显示屏的引脚对应关系  SCL=P2^1; 串行时钟   SDA=P2^0; 串行数据---在头文件OLED.H中 13 14行修改----------------------*/
/* ------------------------------远控配合微信小程序《精灵物联网》--------------------------------------------------------------------------*/


/* ------------------------------欢迎技术交流--------------------------------------------------------------------------*/
/* ---------------------------- QQ群:630017549-------------------------------------------------------------------------*/
/* -----------------------------微信: fairycloud2035-----------------------------------------------------------------------*/
/* -------------------------------QQ:1055417926------------------------------------------------------------------------*/

/* ------------------------------------ END ---------------------------------------------------------------------------*/
 

#include "STC15W4K58S4.h"
#include "DHT11.h"
#include "intrins.h"
#include "stdio.h"
#include "OLED/oled.h"

#include "config.h"
#include "delay.h"

//灯和按钮定义
sbit Buzzer = P4^1;  // Buzzer蜂鸣器
sbit LED = P2^5;  // LED台灯
sbit LED2 = P2^6;
sbit LED3 = P2^7;
sbit SW1 = P5^3;
sbit SW2 = P0^5;
sbit SW3 = P0^6;

u8 xdata RX1_Buffer[450];	//接收缓冲
u8	RX1_Cnt;	//接收计数
u8 mqtt_message[256];//MQTT发送缓冲

bit busy;

typedef char I8;
typedef int I16;
typedef long I32;
typedef unsigned char U8; 

//记录设备的所有状态
static unsigned char DeviceStatus = 0;


static	  unsigned char   AutoSendMsgFlag = 0;// flag

unsigned int TEMP = 0; 
unsigned int HUMI = 0; 

static	 unsigned int   Timer4_Count=1;
static	 unsigned int   Timeout_Count=0;
static	 unsigned int   DisplayTime_Count=0;
static   unsigned char i;

#define FOSC 11059200L          //系统频率
#define BAUD 115200             //串口波特率

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


void DELAY_MS(unsigned int timeout);		//@11.0592MHz   1ms
void DELAY_1MS() ;
void UART_TC (unsigned char *str);//调用下面的UART_T发送
void UART_T (unsigned char UART_data); //定义串口发送数据变量
void UART_R();//接受数据
void ReConnectServer();//重启WIFI连接服务器
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
		
		Device_Init();//初始化硬件


		OLED_Init();
	  OLED_Clear(0);

		OLEDFunc(0,0);
			OLED_ShowString(0,0,"0000/00/00 00:00",16);

		OLED_ShowString(0,7,"Starting        ",8);//connected closed starting


    USART1_Init();//初始化与WiFi通信的串口

		if(PCON&0x10){	//如果是硬启动(上电启动)的话，就进行WiFi的第一次初始化操作，若是热启动（复位启动或看门狗启动）的话直接跳过；因为WiFi在第一次初始化的时候，就进行了“ 保存TCP连接到flash，实现上电透传”
			PCON&=0xef;
			ReConnectServer();
			ESP8266_SmartConnect();
		}

				
		Timer4Init();
		Timer0Init();

	  WDT_CONTR = 0x06;       //看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
                            //设置看门狗定时器分频数为32,溢出时间如下:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //启动看门狗  STC单片机的看门狗一旦启动后，就没法关闭

    while(1) {
			WDT_CONTR |= 0x10;  //喂狗程序
			
			if(DHT11_Read_Data(&DATA_Temphui[0],&DATA_Temphui[1])==0)//温湿度检测
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

//OLED屏幕显示
void OLEDFunc(unsigned char TEMP,unsigned char HUMI){
			//OLED_ShowString(0,0,"0000/00/00 00:00",16);
	
	//温湿度显示
		OLED_ShowString(0,2,"Temp:",16);
		OLED_ShowString(72,2,"Humi:",16);
		
	  OLED_ShowChar(40,2,'0'+TEMP/10%10,16);
	  OLED_ShowChar(48,2,'0'+TEMP%10,16);		
		OLED_ShowChar(112,2,'0'+HUMI/10%10,16);
	  OLED_ShowChar(120,2,'0'+HUMI%10,16);


	
		 OLED_ShowCHinese(0,4,0);//台灯
		 OLED_ShowCHinese(16,4,1);

		 OLED_ShowCHinese(72,4,2);//告警
		 OLED_ShowCHinese(88,4,3);


		 OLED_ShowChar(32,4,':',16);
		 OLED_ShowChar(104,4,':',16);	

		if(!LED){//显示台灯状态 开/关
				OLED_ShowCHinese(40,4,4);
		}else{
				OLED_ShowCHinese(40,4,5);
		}
		
		if(!Buzzer){//显示告警状态 开/关
				OLED_ShowCHinese(112,4,4);
		}else{
				OLED_ShowCHinese(112,4,5);
		}

//异常状态显示
		if(DeviceStatus==0){
				OLED_ShowString(0,7,"OK              ",8);
		}else{
			Enter_ErrorMode(DeviceStatus);
		}
		

}


void AutoSendMsg(){
	
	   unsigned char *strLED ;
	   unsigned char *strBuzzer ;
	
		 //刷新一下LED屏幕
		OLED_Init();
		OLED_Clear(0);
	
		if(!LED){//显示台灯状态 开/关
				strLED = "open";
		}else{
				strLED = "close";
		}
		
		if(!Buzzer){//显示告警状态 开/关
				strBuzzer = "open";
		}else{
				strBuzzer = "close";
		}
		
		


			//注意数据长度不要超过mqtt_message的256 目前是219
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



//初始化LED台灯和Buzzer告警 //0是开启 1是关闭
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
    SCON = 0x50;                //8位可变波特率
    PS = 1;
		
	
    AUXR = 0x40;                //定时器1为1T模式
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    ES = 1;                     //使能串口中断
    EA = 1;

}



/*----------------------------
UART 中断接收服务程序
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
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}

//串口  接收ESP8266的串口数据，并校验数据的完整性9位

void UART_R()
{
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xDC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;	//允许中断
	
	
	RX1_Buffer[RX1_Cnt] = SBUF;
	if(++RX1_Cnt >= 450)	RX1_Cnt = 0;

}


//串口发送数据字符数组遇到\0停止

void UART_TC (unsigned char *str) {
	 EA = 0;
    while(*str != '\0') {
        UART_T(*str);
        *str++;
    }
    *str = 0;
		 EA = 1;
}

void UART_T (unsigned char UART_data) { //定义串口发送数据变量
    SBUF = UART_data;	//将接收的数据发送回去
    while(!TI);		//检查发送中断标志位
    TI = 0;			//令发送中断标志位为0（软件清零）
}



//重启 ESP8266WiFi模块
void ReConnectServer() {

    UART_TC("AT+RST\r\n\0");  // 复位
			
}


void Timer4Init(void)		
{
		//50 毫秒@11.0592MHz
		T4T3M &= 0xDF;		//定时器时钟12T模式
		T4L = 0x00;		//设置定时初值
		T4H = 0x4C;		//设置定时初值
		T4T3M |= 0x80;		//定时器4开始计时
	
		IE2 |= 0x40;		//开定时器4中断
		EA=1; 	//总中断开启
}

//10s中断自动上报信息
void Timer4_interrupt() interrupt 20    //定时中断入口
	{
		
		if(DisplayTime_Count>=20){  //20 * 50ms = 1s
			DisplayTime_Count = 0;
			
			
		}else{
			 DisplayTime_Count++;//每加一次加50ms
		}
				
		if(Timer4_Count>=200){  //200 * 50ms = 10s
			AutoSendMsgFlag = 1;
			Timer4_Count = 0;

//				Timeout_Count++;//每加一次加10s
//				
//				if(Timeout_Count < 3 ){ //没有认证成功发了服务器也会拒绝
//					//  AutoSendMsgFlag = 1;

//				}else if(Timeout_Count >= 3){//30s还是收不到服务器返回的数据，则 重启机器
//						//重新链接平台
//					//ReConnectServer();
//					Timeout_Count = 0;
//					OLED_ShowString(0,7,"Closed          ",8);//connected closed starting

//				}
						
		}else{
						
				Timer4_Count++;
		}
					

		
}






void Timer0Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xB8;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;	//允许中断
}



/********************* Timer0中断函数************************/
void timer0_int (void) interrupt 1
{
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xB8;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0开始计时
	ET0 = 0;	//允许中断
	
	CloudHandleReceive();
	RX1_Cnt = 0;
}


void Led_Actions_Status(unsigned char status){  //0是开启 1是关闭

	if(status){
		LED = 1;
	}else{
		LED = 0;
	}

}

void Buzzer_Actions_Status(unsigned char status){ //0是开启 1是关闭

	if(status){
		Buzzer = 1;
	}else{
		Buzzer = 0;
	}

}

/******************************  智能一键配网模式  *****************************/
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
				//PrintString1("\模式选择成功\r\n" );
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
	if(status==3)//配置WiFi模组工作模式为单STA模式，并把配置保存在flash
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
				//UART_TC("\r重新联网成功SUCCESS\r\n\0" );
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
			}
			else
			{
				//UART_TC("\r重新联网FAIL\r\n\0" );
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
	while(status==5)//配置WiFi模组工作模式为单STA模式，并把配置保存在flash
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

/******************************  进入错误模式代码  *****************************/

//进入错误模式等待手动重启
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
	

    if (strncmp(RX1_Buffer, "ERROR", 5) == 0) //命令执行异常
    {
        //目前不处理,待完善
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
    }
    else if (strncmp(RX1_Buffer, "OK", 2) == 0) //命令执行正常
    {
				memset(RX1_Buffer,0,450);
				RX1_Cnt = 0;
    }
    else if (RX1_Buffer[0] == '+') //读取详细信息
    {
			

						if (strncmp(RX1_Buffer + 1, "MQTTSUBRECV", 11) == 0) //收到订阅信息
						{

								pdata U8 *P=RX1_Buffer;//从这里开始查找长度
								pdata U8 name[20],i=0;
								pdata U8 reporttime[16];
							  memset(name,0,20);
							  memset(reporttime,0,16);
							  i=0;
					      
							
							//匹配第一次出现的地址  状态查询
								P=strstr(P,"\"cmdtype\":\"cmd_status\"");
										
							  if(P != NULL){
												sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
													
													AutoSendMsgFlag = 1;

								}
							
							
								i=0;
								P=RX1_Buffer;

								
							 //匹配第一次出现的地址  显示返回的时间戳
								P=strstr(P,"\"reporttime\":\"");
										
							  if(P != NULL){
											P+=14;
                      while(*P != '"'&&i!=16)
												reporttime[i++]=*P++;										
			            OLED_ShowString(0,0,reporttime,16);

								}
							
							
								i=0;
								P=RX1_Buffer;

								//匹配第一次出现的地址  匹配远控指令
								P=strstr(P,"\"sensorname\":\"");
								
								//没有匹配到远控指令
							  if(P == NULL){

										memset(RX1_Buffer,0,450);
										RX1_Cnt = 0;
										memset(name,0,20);
										i=0;
									
								//匹配到远控指令
								}else{
									
											P+=14;
											//远控指令名称
											while(*P != '"'&&i!=19)
												name[i++]=*P++;
											name[i]=0;
										//状态查询
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
										//重启
											else if(strncmp(name,"restart",7) == 0){
											
													sprintf(mqtt_message,"AT+MQTTPUB=0,\"%s\",\"{\\\"status\\\":\\\"success\\\"\\\,\\\"cmdtype\\\":\\\"cmd_controllack\\\"\\\,\\\"did\\\":\\\"%s\\\"\\\,\\\"cid\\\":\\\"%s\\\"\\\,\\\"version\\\":\\\"%s\\\"}\",0,0\r\n\0",DPubTopic,Ddid,DCID,Dversion);
			                    UART_TC(mqtt_message);
												
													IAP_CONTR = 0X20;
											}	
											//灯
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
											//告警
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
    else //读取附加信息
    {
        //目前用不到
    }
}


