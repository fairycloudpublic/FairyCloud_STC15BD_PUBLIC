#ifndef __CONFIG_H
#define __CONFIG_H

#include "STC15W4K58S4.h"
#include <string.h>


// 1.你的设备CID，17位设备编码
unsigned char SRCCID[] = {"SXXXXXXXXXXXXXXXX"};

// 2.填写你的WiFi名称和密码，示例的WiFi名称 Fariy    密码 qwerty123
unsigned char netConfig[] = "AT+CWJAP=\"Fariy\",\"qwerty123\"\r\n\0";

// 3.填写32位openid，平台管理员直接提供给你！
unsigned char SRCOPENID[] = {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"};

// 4.填写服务器的IP和端口，平台管理员直接提供给你！
unsigned char TcpServer[] = "AT+CIPSTART=\"TCP\",\"xxxxxxxxxxxxxxxxx\",xxxxx\r\n\0";
unsigned char SaveTcpServer[] = "AT+SAVETRANSLINK=1,\"xxxxxxxxxxxxxxxxx\",xxxxx,\"TCP\"\r\n\0";


#endif
