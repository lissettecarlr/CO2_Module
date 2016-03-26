#include "stdlib.h"
#include "ADC.h"
#include "GPIO.h"
//#include "Timer.h"
#include "USART.h"
#include "Memory.h"
#include "Interrupt.h"
#include "TaskManager.h"
#include "transmission.h"
#include "ESP8266.h"
#include "Hint.h"
#include "WIFI_Memory.h"
#include "MHZ14.h"

/*DEFINE********************************************************/
#define DELAY 					0x01
#define START 					0x02
#define GETWIFI					0x03
#define REGISTER 				0xaa
#define BEEP 						0xBB
#define RESET 					0XCC
#define MODEL 					0xEE
#define ALIVE 					0xff


#define MODULE_IP 			"192.168.1.1"   
#define MODULE_COM			9000
#define SERVER_IP				"120.27.119.115"
#define SERVER_COM 			1111
/*END********************************************************/


USART com(1,115200,true);   //USART1
USART Mhz14(2,9200,true); 
USART WIFI(3,115200,true);   //USART3

MHZ14 co2(Mhz14);

GPIO Beep(GPIOA,11,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
GPIO Led(GPIOB,0,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);

esp8266 wifi(WIFI);


Memory InfoStore(0x08000000+100*MEMORY_PAGE_SIZE,true);//ip存储，长度+IP地址的存档格式，先读取出字符串长度
WifiMemory wifimemory(InfoStore);
hint Hint(Led,Beep);


int main(){
	u8 order=0;
	double record_updataSensor=0;
	double record_alive=0;
	double record_getwifi=0;
	bool network=false ; //网络连接标识位 
	char *ip = (char*)calloc(15, sizeof(char*) ); 
	char *WifiName = (char*)calloc(20, sizeof(char*) ); 
	char *WifiPassword = (char*)calloc(20, sizeof(char*) ); 
	
/*开机WIFI模式选择*****************************************************************/
	if(wifimemory.getWifiSum()!=0)//判断表中是否已经保存wifi信息
	{
		while( wifimemory.Load(WifiName,WifiPassword) )
		 {
			  if(wifi.ConnectNetwork_client(WifiName,WifiPassword,SERVER_IP,1111)) //每次连接历时20秒
				{network=true;   break;}
		 }
		 if(network ==false) {
			  wifi.ConnectNetwork_server(MODULE_COM,0);
			 	WIFI.ClearReceiveBuffer();
		 }
	}
  else
	{
		 wifi.ConnectNetwork_server(MODULE_COM,0);
			WIFI.ClearReceiveBuffer();
	}
	
	Led.SetLevel(1);//将指示灯熄灭（模式切换结束）
/*END*********************************************************************************/	

while(1)
	{		
		order=CMCT_Tool.GetStateOrder(WIFI);
		switch(order)
		{
			case DELAY:{}break;
	/*启动********************************************************************************************/
			case START:{//启动
				while(1){
							if(tskmgr.ClockTool(record_updataSensor,3)) //每三秒执行一次更新
							{
								co2.Updata();
								if(network)
									wifi.Send(20,CMCT_Tool.ToServerPack(CO2Number,co2.DATA_H,co2.DATA_L,0));	
								else
									wifi.Send(0,20,CMCT_Tool.ToClientPack(CO2Number,co2.DATA_H,co2.DATA_L,0));	
							}
							if(CMCT_Tool.GetStateOrder(WIFI)==DELAY)
							break;					
				}
			}break;
	/*END********************************************************************************************/			
			
	/*复位******************************************************************************************/
			case RESET:{//复位
					wifimemory.ClearAllData(); //清空所有保存信息
					*((u32 *)0xE000ED0C) = 0x05fa0004;
			}break;
	/*END******************************************************************************************/
			
	/*获取WIFI信息*********************************************************************************/		
			case GETWIFI:{//得到wifi账号密码
				record_getwifi=tskmgr.Time();
					while(1)
					{
						if(CMCT_Tool.GetWifiNameAndPassword(WifiName,WifiPassword,WIFI) )
						{
								//保存WIFI账号密码
								com<<WifiName<<"\t"<<WifiPassword<<"\n";
								break;
						}
						if(tskmgr.ClockTool(record_getwifi,30)) //超时10秒退出
						   break;
					}
			}break;
	/*END******************************************************************************************/
			
	/*模式切换*************************************************************************************/
			case MODEL:{//模式切换
					wifi.ConnectNetwork_server(MODULE_COM,0);
					WIFI.ClearReceiveBuffer();
					network=false;
			}break;
	/*END******************************************************************************************/

	/*存活确认*************************************************************************************/			
			case ALIVE:{//存活确认
				if(network)
					CMCT_Tool.SendAlive(wifi,CO2Number,1); //存活确认，数据位全为0xff
				else
					CMCT_Tool.SendAlive(wifi,CO2Number,0);
			}break;			
	/*END******************************************************************************************/		
			
	/*常态******************************************************************************************/		
			default:		
			{  
					if(tskmgr.ClockTool(record_updataSensor,3)) //每三秒执行一次更新
						 co2.Updata();//当没有命令时更新传感器
						if(tskmgr.ClockTool(record_alive,60)) //每60秒发送一次存活确认
					{
						if(network)
							CMCT_Tool.SendAlive(wifi,CO2Number,1);
						else
							CMCT_Tool.SendAlive(wifi,CO2Number,0);
					}
			}
		}		
	}
}

//ff dd 00 00 00 02 02 E0
