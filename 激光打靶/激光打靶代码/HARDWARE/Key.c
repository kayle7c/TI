#include "KEY.h"
#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"

int q3;

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

int KEY_Num()
{
	int t=0;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 1)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 1);	
		delay_ms(20);
		t=1;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 1)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==1);	
		delay_ms(20);
		t=2;
	}
	return t;
}