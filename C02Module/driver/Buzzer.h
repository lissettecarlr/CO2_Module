#ifndef _BSP_BUZZER_H
#define _BSP_BUZZER_H

extern "C"{
#include "stm32f10x.h"
}

#define BUZZER_RCC RCC_APB2Periph_GPIOA    //蜂鸣器使用的时钟
#define BUZZER_GPIO GPIOA                  //管脚
#define BUZZER_PIN GPIO_Pin_4


class Buzzer{
 
	public:
		Buzzer(void);
	  void BUZZER_OFF(void)  
		{ GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);}
	  void BUZZER_ON(void)
		{ GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);}
	private:
		
} ;



#endif
