#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //echo pinA.6
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3); //选择时钟（内部）
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//配置时基单元
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //预分频系数（1，2，4）
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStruct.TIM_Period = 65536-1;      //ARR（自动重装载值）0~65535
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;    //PSC（预分频值）    0~65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //选择通道
	TIM_ICInitStructure.TIM_ICFilter = 0xF; //配置滤波器，参数越大过滤效果越好
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising ; //极性，触发方式
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1 ;//分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI ;//指定输入，是直连还是交叉
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	TIM_PWMIConfig(TIM3,&TIM_ICInitStructure);
	
	
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM3,ENABLE);
}

uint32_t IC_GetDistance(void)
{
	return ((TIM_GetCapture2(TIM3)+1)*0.017)*10;//返回实际cm的10倍
}
