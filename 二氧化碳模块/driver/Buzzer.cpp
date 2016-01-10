#include "Buzzer.h"


Buzzer::Buzzer(void)
{
  GPIO_InitTypeDef GPIO_Structure;
	
	RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);
	
	GPIO_Structure.GPIO_Pin =  BUZZER_PIN;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(BUZZER_GPIO, &GPIO_Structure);
	
	GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
}

