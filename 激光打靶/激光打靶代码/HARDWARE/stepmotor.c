#include "stepmotor.h"
#include "stm32f10x.h"


float speed = 0;

//默认配置：
//EN:高电平有效
//DIR：高电平顺时针
void HGPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
}

void HGPIOx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}

void PWM_Init(void)
{
	/*
	1.RCC开启时钟
	2.配置时基单元
	3.配置输出比较单元（CCR输出比较器，极性选择，输出使能）
	4.配置GPIO
	5.启动计数器
	*/
	//1打开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//2选择时基单元的时钟，一般默认为内部时钟
	TIM_InternalClockConfig(TIM2);
 
	//3配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
 
 
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1 ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
 
	TIM_CtrlPWMOutputs(TIM3,ENABLE);	               //
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //
	TIM_ARRPreloadConfig(TIM3, ENABLE);                //
	TIM_Cmd(TIM3, ENABLE);
 
}

void PWMx_Init(void)
{
	/*
	1.RCC开启时钟
	2.配置时基单元
	3.配置输出比较单元（CCR输出比较器，极性选择，输出使能）
	4.配置GPIO
	5.启动计数器
	*/
	//1打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//2选择时基单元的时钟，一般默认为内部时钟
	TIM_InternalClockConfig(TIM2);
 
	//3配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
 
 
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1 ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
 
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	               //
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //
	TIM_ARRPreloadConfig(TIM1, ENABLE);                //
	TIM_Cmd(TIM1, ENABLE);
 
}


//电机转速设置
//n:			转速 （n r/s）
void STEPMOTOR_Set_NY(float n)
{
	if(n == 0)
	{
		TIM_SetCompare1(TIM3,0);	
	}
	else{
		int t = (STEPMOTOR_STEP_ANGLE * 1000000.0 / (STEPMOTOR_SUBDIVISION * 360 * n));

		TIM_SetAutoreload(TIM3,t);
		TIM_SetCompare1(TIM3,t/2);	

	}
}

void STEPMOTOR_Set_NX(float n)
{

	if(n == 0)
	{
		TIM_SetCompare1(TIM1,0);	
	}
	else{
		int t = (STEPMOTOR_STEP_ANGLE * 1000000.0 / (STEPMOTOR_SUBDIVISION * 360 * n));

		TIM_SetAutoreload(TIM1,t);
		TIM_SetCompare1(TIM1,t/2);	
	}

}


//电机方向设置
//mod:		电机方向（0-停机 ； 1-顺时针转动 ； -1-逆时针转动）
void STEPMOTOR_Dir_ColY(int mod)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	switch (mod)
		{
			case 0:   //停机
				GPIO_ResetBits(GPIOB,GPIO_Pin_0);
				break;
			case -1:  //顺时针
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				break;
			case 1: //逆时针
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				break;
		}
	
}

void STEPMOTOR_Dir_ColX(int mod)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	switch (mod)
		{
			case 0:   //停机
				GPIO_ResetBits(GPIOB,GPIO_Pin_14);
				break;
			case 1:  //顺时针
				GPIO_SetBits(GPIOB,GPIO_Pin_15);
				break;
			case -1: //逆时针
				GPIO_ResetBits(GPIOB,GPIO_Pin_15);
				break;
		}
	
}

void Set_x(float  n)
{
	if(n>=0)
	{
		STEPMOTOR_Dir_ColX(1);
		STEPMOTOR_Set_NX(n);
	}
	else{
		STEPMOTOR_Dir_ColX(-1);
		STEPMOTOR_Set_NX(-n);
	}
}

void Set_y(float n)
{
	if(n>=0)
	{
		STEPMOTOR_Dir_ColY(1);
		STEPMOTOR_Set_NY(n);
	}
	else{
		STEPMOTOR_Dir_ColY(-1);
		STEPMOTOR_Set_NY(-n);
	}
}



