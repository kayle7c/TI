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
	
	TIM_InternalClockConfig(TIM3); //ѡ��ʱ�ӣ��ڲ���
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//����ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //Ԥ��Ƶϵ����1��2��4��
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStruct.TIM_Period = 65536-1;      //ARR���Զ���װ��ֵ��0~65535
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;    //PSC��Ԥ��Ƶֵ��    0~65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //ѡ��ͨ��
	TIM_ICInitStructure.TIM_ICFilter = 0xF; //�����˲���������Խ�����Ч��Խ��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising ; //���ԣ�������ʽ
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1 ;//��Ƶ
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI ;//ָ�����룬��ֱ�����ǽ���
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	TIM_PWMIConfig(TIM3,&TIM_ICInitStructure);
	
	
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM3,ENABLE);
}

uint32_t IC_GetDistance(void)
{
	return ((TIM_GetCapture2(TIM3)+1)*0.017)*10;//����ʵ��cm��10��
}
