# FairyCloud_STC15BD_PUBLIC
STC15物联网核心板-硬件代码;欢迎交流：QQ群：630017549，个人微信：fairycloud2035


【MQTT版本】STC15核心板代码，实现MQTT连接平台，支持小程序远程查看温度、湿度、灯、蜂鸣器状态，以及远程控制灯和蜂鸣器；


## 代码说明
### 代码目录
FairyCloud_STC15BD_PUBLIC/code/


### 配置文件目录
FairyCloud_STC15BD_PUBLIC/code/config.h


### 配置文件说明
需要修改配置的地方如下：目录在code/config.h

----------------WIFI热点名称及密码------------------------
//WiFi名称
#define WiFiName "XXXXXXXXXXX"
//WiFi密码
#define WiFiPassword "XXXXXXXXXXX"

----------------精灵物联网参数------------------------
//MQTT地址
#define mqttHostUrl "XXXXXXXXXXX"
//MQTT端口
#define port 1883
//设备CID
#define DCID "SRCTDSPWU000000XX"
//MQTT name
#define DName "SRCTDSPWU000000XX&STC15DB"
//MQTTclientid---一定注意，中间的逗号前面，自己加两个斜杠！！！
#define DClientID "16731950792|securemode=2\,signmethod=hmacsha256\,timestamp=1723389081212|"
//MQTT密码
#define DPassWord "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
//MQTT订阅主题
#define DSubTopic "server/home/SRCTDSPWU000000XX"
//MQTT发布主题
#define DPubTopic "client/home/SRCTDSPWU000000XX"


### 运行

第一次运行大约17s左右，设备登录到平台



## 示例教程

### 实物演示
[【STC15物联网核心板，小程序远程控制-哔哩哔哩】](https://b23.tv/LC0sZ2T)

### 说明文档
[【外部】精灵物联网各项目汇总](https://gv9jqt8gpcb.feishu.cn/docx/DAJGdExvZoZBA3xuAogc53ohnxg?from=from_copylink)

### 实物图片
![image](https://github.com/fairycloudpublic/FairyCloud_STC15BD_PUBLIC/blob/main/photo1.png)
![image](https://github.com/fairycloudpublic/FairyCloud_STC15BD_PUBLIC/blob/main/photo2.png)
![image](https://github.com/fairycloudpublic/FairyCloud_STC15BD_PUBLIC/blob/main/photo3.png)


## 版权说明
仅供大家学习与参考，未经版权所有权人书面许可，不能自行用于商业用途。如需作商业用途，请与原作者联系。

### 许可协议
许可协议 AGPL3.0协议

### 软著证书
![image](https://github.com/fairycloudpublic/FairyCloud_STC15BD_PUBLIC/blob/main/%E7%B2%BE%E7%81%B5%E7%89%A9%E8%81%94%E7%BD%91%E5%B9%B3%E5%8F%B0%E7%89%88%E6%9D%83.png)
