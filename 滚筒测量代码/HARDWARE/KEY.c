#include "stm32f10x.h"                  // Device header

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
		
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
}


int key_scan(void)
{
	int t=0;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0);	
		delay_ms(20);
		t=1;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0);	
		delay_ms(20);
		t=2;
	}
	return t;
	
}