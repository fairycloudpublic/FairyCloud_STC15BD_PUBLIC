# FairyCloud_STC15BD_PUBLIC
STC15物联网核心板-硬件代码;欢迎交流：QQ群：630017549，个人微信：fairycloud2035

## 示例教程
### 实物演示
【STC15物联网核心板，小程序远程控制-哔哩哔哩】 https://b23.tv/LC0sZ2T

### 说明文档
https://gv9jqt8gpcb.feishu.cn/docx/VRTfdDqVZoiFHJxuQq2cCiFmnFc?from=from_copylink

## 代码说明
### 代码目录
FairyCloud_STC15BD_PUBLIC/code/
### 配置文件目录
FairyCloud_STC15BD_PUBLIC/code/config.h
### 配置文件说明

1.你的设备CID，17位设备编码
unsigned char SRCCID[] = {"SXXXXXXXXXXXXXXXX"};

2.填写你的WiFi名称和密码，示例的WiFi名称 Fariy    密码 qwerty123
unsigned char netConfig[] = "AT+CWJAP=\"Fariy\",\"qwerty123\"\r\n\0";

3.填写32位openid，平台管理员直接提供给你！
unsigned char SRCOPENID[] = {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"};

4.填写服务器的IP和端口，平台管理员直接提供给你！
unsigned char TcpServer[] = "AT+CIPSTART=\"TCP\",\"xxxxxxxxxxxxxxxxx\",xxxxx\r\n\0";
unsigned char SaveTcpServer[] = "AT+SAVETRANSLINK=1,\"xxxxxxxxxxxxxxxxx\",xxxxx,\"TCP\"\r\n\0";


## 版权说明
仅供大家学习与参考，未经版权所有权人书面许可，不能自行用于商业用途。如需作商业用途，请与原作者联系。

### 许可协议
许可协议 AGPL3.0协议
### 软著证书
![image](https://github.com/fairycloudpublic/FairyCloud_STC15BD_PUBLIC/blob/main/%E7%B2%BE%E7%81%B5%E7%89%A9%E8%81%94%E7%BD%91%E5%B9%B3%E5%8F%B0%E7%89%88%E6%9D%83.png)
