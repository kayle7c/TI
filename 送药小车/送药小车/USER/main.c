#include "sys.h"
#include "delay.h"
#include "OLED.h"
#include "PWM.h"
#include "MOTOR.h"
#include "ADC.h"
#include "usart.h"
#include "pic.h"
#include "led.h"
#include "lcd.h"
#include "lcd_init.h"
#include "encorder.h"
#include "pid.h"

int adc1,adc2,adc3,adc4;
int cross,number;
int dif;
int turn,x1,x2;
int direction,LR;
char tmp;
int turn_flag;
int first_turn=-1;

void get_data(void);


int main()
{
	Encoder_Init();
	uart_init(9600);
	MOTOR_Init();
	MOTOR_PWM_Init();  
	Adc_Init();         
	delay_init();
	LED_Init();//LED初始化
	LCD_Init();//LCD初始化
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	while(1)
	{
		//LCD_ShowChar(1,1,'a',RED,WHITE,16,0);
		get_data();
		LCD_ShowIntNum(1,18,turn,3,RED,WHITE,16);
			LED=~LED;
	}
	while(1)
	{
		get_data();
		if(cross==1)
		{
				PWM_SetCompare3(0);									//right
				PWM_SetCompare4(0); 
		}
		//show_adc();
		get_grayscale();
		MOTOR_Front();
		
		if(adc1<900 && adc2<900 && adc3<900 && adc4<900)
		{
			turn_flag=1;
			first_turn=LR;
		}
		if(first_turn == 0)
		{
			MOTOR_L();
		}
		if(first_turn == 1)
		{
			MOTOR_R();
		}
		
		
		if(direction==1 && LR!=0)
		{
			get_data();
			MOTOR_Behind();
			if(turn_flag==1 && adc1<900 && adc2<900 && adc3<900 && adc4<900)
			{
					if(LR=='L')
					{
							MOTOR_L();
							LR=0;
					}
					if(LR=='R')
					{
							MOTOR_R();
							LR=0;
					}
			}
		}
		
//		printf("x1 = %d  x2 = %d  turn = %d\r\n",x1,x2,turn);
	}
}


void get_data()
{
		if(((USART_RX_STA&0x8000)>>15)==1)
		{
			if(USART_RX_BUF[0]==0xA0)
			{
//				direction=USART_RX_BUF[1];
//				LR=USART_RX_BUF[2];
				x1=USART_RX_BUF[1];
				x2=USART_RX_BUF[2];
//				cross=USART_RX_BUF[5];
				if(x1>127) x1=(256-x1)*-1;
				turn = x1*100+x2+160;
			}		
			
		printf("x1 = %d  x2 = %d  turn = %d\r\n",x1,x2,turn);
			USART_RX_STA=0;
		}
}

