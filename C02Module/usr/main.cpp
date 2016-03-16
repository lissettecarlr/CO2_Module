#include "ADC.h"
#include "USART.h"
#include "Interrupt.h"
#include "TaskManager.h"
#include "transmission.h"
#include "MHZ14.h"
#include "Memory.h"
#include "stdlib.h"
#include "WIFI_Memory.h"
#include "GPIO.h"
#include "ESP8266.h"

/*DEFINE********************************************************/
#define DELAY 					0x01
#define SEND_ONCE 			0x02
#define REGISTER 				0xaa
#define DELETE 					0xdd
#define BEEP 						0xBB
#define RESET 					0XCC
#define ALIVE 					0xff

/*END********************************************************/


USART com(1,115200,false);   //USART1
MHZ14 co2;
USART WIFI(3,115200,false);   //USART3
esp8266 wifi(WIFI);
u8 ConnectNetwork();

int main(){
		u8 order=0;
		ConnectNetwork();
		while(1)
	 {
//		tskmgr.DelayMs(300); //延时是必要的！
		order=CMCT_Tool.GetStateOrder(WIFI);
		switch(order)
		{
			case DELAY:{}break;
			
			case SEND_ONCE:{
				  co2.Updata();
				  com.SendData(CMCT_Tool.CO2_ModuleToUser(co2.DATA_H,co2.DATA_L,0),20);
				  wifi.Send(0,20,CMCT_Tool.CO2_ModuleToUser(co2.DATA_H,co2.DATA_L,0));
			}break;
			
			case REGISTER:{}break;
			case DELETE:{}break;
			case BEEP:{}break;
			case RESET:{}break;
			case ALIVE:{
				CMCT_Tool.SendOnce(0xff,0xff,0xff,0xff,0xff,com,wifi); //存活确认，数据位全为0xff
			}break;			
			default:{}
			
		}
	}
}


//IP地址192.168.1.1:8888
u8 ConnectNetwork()
{
		if(!wifi.kick())
		return 0;
		tskmgr.DelayMs(1000);
		wifi.setOprToSoftAP();
		tskmgr.DelayMs(1000);
		wifi.enableOrDisableMUX(1); //开启多路访问
		tskmgr.DelayMs(1000);
		WIFI<<"AT+CIPSERVER=1,8888"<<"\r\n";  //开启服务
		tskmgr.DelayMs(1000);
		WIFI<<"AT+CIPSTO=0"<<"\r\n";  //设置永不超时
		tskmgr.DelayMs(1000);
		WIFI.ClearReceiveBuffer();
}



//ff dd 00 00 00 02 02 E0