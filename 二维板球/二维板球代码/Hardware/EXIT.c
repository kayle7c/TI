#include "stm32f10x.h"                  // Device header

#include "EXIT.h"
#include "KEY.h"
#include "delay.h"
int temp=1;

void EXTIX_Init(void)
{
	KEY_Init();
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitStruct.EXTI_Line=EXTI_Line4;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
//	EXTI_InitStruct.EXTI_Line=EXTI_Line4;
//  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	
//  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =1;
 	NVIC_InitStruct.NVIC_IRQChannelSubPriority =1;
 	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI4_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line4)!= RESET)
	{
		temp=0;
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

//void EXTI9_5_IRQHandler()
//{
//	if(EXTI_GetITStatus(EXTI_Line5)!= RESET)
//	{
//		
//		EXTI_ClearITPendingBit(EXTI_Line5);
//	}
//}

