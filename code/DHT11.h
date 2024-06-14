#ifndef __DHT11_H_
#define __DHT11_H_

//extern unsigned char DHT11();
extern unsigned char DHT11_Read_Data(unsigned char *temp,unsigned char *humi); 
extern  unsigned char xdata DATA_Temphui[3];   //ÎÂÊª¶È
//extern void DELAY_CMS(unsigned char ms);
#endif