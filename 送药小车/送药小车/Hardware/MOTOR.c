#include "stm32f10x.h"
#include "MOTOR.h"
#include "PWM.h"
#include "led.h"
#include "lcd.h"
#include "ADC.h"

extern int adc1,adc2,adc3,adc4;
extern int turn;
extern int LR;
extern int first_turn;

void MOTOR_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; //rIGHT
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void MOTOR_Front(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
	
	PWM_SetCompare3(1000+turn);							//right
	PWM_SetCompare4(1000); 
}

void MOTOR_Behind(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	
	PWM_SetCompare3(1000+turn);							//right
	PWM_SetCompare4(1000); 
}

void MOTOR_L(void)
{
	int time=0;
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
	show_adc();
	while(time<120)
	{
		PWM_SetCompare3(1500);							//right
		PWM_SetCompare4(650);  
		time++;
		LCD_ShowIntNum(1,68,time,4,RED,WHITE,16);
	}
	first_turn=-1;
}

void MOTOR_R()
{
	int time=0;
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	show_adc();
	while(time<100)
	{
		PWM_SetCompare3(650);							//right
		PWM_SetCompare4(1500);  
		time++;
		LCD_ShowIntNum(1,68,time,4,RED,WHITE,16);
	}
	first_turn=-1;
}	

