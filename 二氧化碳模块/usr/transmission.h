#ifndef __TRANSMISSION_H
#define __TRANSMISSION_H

#include "stm32f10x.h"

class Transmission{
	
	private:
		u8 ModuleToModule[20];
		u8 ModuleToUser[20];
	public:
		Transmission();
		u8 *C02_ModuleToModule(u8 data1,u8 data2,u8 adc);

		u8 *C02_ModuleToUser(u8 data1,u8 data2,u8 adc);
	
		u8 CommandParsing(u8 command[8]);
	
};

extern Transmission packaging;

#endif


/*
接收命令：
FF DD 00 00 00 02 01 DF   //待机
FF DD 00 00 00 02 02 E0   //发送一次
FF DD 00 00 00 02 03 E1   //连续发送

新增： 确认存在 FF DD 00 00 00 02 FF DD

发送数据格式：
一帧数据=2字节包头+4字节设备号+1字节数据类别+2字节数据+1字节电压 +9字节保留+1字节校验


*/
