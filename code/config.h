#ifndef __CONFIG_H
#define __CONFIG_H

#include "STC15W4K58S4.h"
#include <string.h>


//----------------WIFI热点名称及密码------------------------
//WiFi名称
#define WiFiName "XXXXXXXXXXX"
//WiFi密码
#define WiFiPassword "XXXXXXXXXXX"

//----------------精灵物联网参数------------------------
//MQTT地址
#define mqttHostUrl "XXXXXXXXXXX"
//MQTT端口
#define port 1883
//设备CID
#define DCID "SRCTDSPWU000000XX"
//MQTT name
#define DName "SRCTDSPWU000000XX&STC15DB"
//MQTTclientid---一定注意，中间的逗号前面，自己加两个斜杠！！！
#define DClientID "16731950792|securemode=2\\,signmethod=hmacsha256\\,timestamp=1723389081212|"
//MQTT密码
#define DPassWord "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
//MQTT订阅主题
#define DSubTopic "server/home/SRCTDSPWU000000XX"
//MQTT发布主题
#define DPubTopic "client/home/SRCTDSPWU000000XX"


//Ó²¼þ°æ±¾ºÅÂë
#define Dversion "1.0.0"
#define Ddid "75X9T3KR6WJX6PRY76C9AGEBFEAQVG2E"


#endif
