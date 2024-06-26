#include "stm32f10x.h"                  // Device header
#include "delay.h"


void sensor_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//trig pinA.5
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sensor_start(){
		delay_init();
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		delay_us(15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}