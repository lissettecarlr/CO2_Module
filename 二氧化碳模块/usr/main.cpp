#include "ADC.h"
#include "Timer.h"
#include "USART.h"
#include "Interrupt.h"
#include "TaskManager.h"
#include "transmission.h"
#include "MHZ14.h"


//LED pb4


USART com(1,115200,false);   //USART1
MHZ14 co2;
USART WIFI(3,9600,false);   //USART3
//ADC voltage(8);


#define CONTINUOUS 3
#define ONCE 2
#define STANDBY 1
#define ALIVE 0xFF


int main()
{
//	static float newTime = 0, oldTime = 0, interval = 0;
	u8 comand[8];//用于保存命令
	static u8 COMANDS;
	u8 state_now=STANDBY; //用于选择模式
	
	while(1)
	{
		//usart 1
		if(com.ReceiveBufferSize()>7)   //一帧命令包含8个字节 
		{
			com.GetReceivedData(comand,8);//读出命令
			COMANDS=packaging.CommandParsing(comand); //解析命令
			if(COMANDS==2) //表示更新一次
			{
				state_now=ONCE;  
			}
			else if(COMANDS==3)//连续更新
			{
				state_now=CONTINUOUS;
			}
			else if(COMANDS==1)
			{
				state_now=STANDBY; //不更新
			}
			else if(COMANDS==0xff)
			{
			  state_now=ALIVE; //确认存在
			}
		}
		//wifi
		if(WIFI.ReceiveBufferSize()>7)   //一帧命令包含8个字节 
		{
			WIFI.GetReceivedData(comand,8);//读出命令
			COMANDS=packaging.CommandParsing(comand); //解析命令
			if(COMANDS==2) //表示更新一次
			{
				state_now=ONCE;  
			}
			else if(COMANDS==3)//连续更新
			{
				state_now=CONTINUOUS;
			}
			else
			{
				state_now=STANDBY; //不更新
			}
		}
		
		
		
		switch(state_now)
		{
			case ONCE:{
			co2.Updata();
			WIFI.SendData((packaging.C02_ModuleToUser(co2.DATA_H,co2.DATA_L,0xaa)),20);
			com.SendData((packaging.C02_ModuleToUser(co2.DATA_H,co2.DATA_L,0xaa)),20);
			state_now=STANDBY;
			}break;
			
			case CONTINUOUS:{
				co2.Updata();
				WIFI.SendData((packaging.C02_ModuleToUser(co2.DATA_H,co2.DATA_L,0xaa)),20);
				com.SendData((packaging.C02_ModuleToUser(co2.DATA_H,co2.DATA_L,0xaa)),20);
				tskmgr.DelayMs(2000);
			}break;
			case STANDBY:{
				
			}break;
			case ALIVE:{  //确认存在
				WIFI.SendData((packaging.C02_ModuleToUser(0xff,0XFF,0xFF)),20);
				com.SendData((packaging.C02_ModuleToUser(0XFF,0XFF,0xFF)),20); 
				state_now=STANDBY;
			}break;
			
		}   
					
//		tskmgr.DelayMs(1000);
//		tskmgr.DelayMs(1000);
//		tskmgr.DelayMs(1000);
	}

//						newTime = tskmgr.Time(); 
//						interval = newTime - oldTime;
//					if(interval>0.2)	
//					{
//						oldTime = newTime;
//					}						
	
}
	
