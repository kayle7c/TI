#include "stm32f10x.h"
#include "STEPMOTOR.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "pid.h"
#include "OLED.h"
#include "Key.h"
 
 
extern u8  USART_RX_BUF[];  
extern u16 USART_RX_STA;         		
int x,y;
int set_d_x,set_d_y;
int location=0;
int m=4000;
int key_num=0;
int flag=1;


void get_data()
{
	if(((USART_RX_STA&0x8000)>>15)==1)
	{
	//	printf("ok");
		
		if(USART_RX_BUF[0]==0xA0)
		{
				x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
				y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
				set_d_x=USART_RX_BUF[5]*100+USART_RX_BUF[6];
				set_d_y=USART_RX_BUF[7]*100+USART_RX_BUF[8];		
		}	
		USART_RX_STA=0; 		
	}
	
}	
 
 int main(void)
 {	
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(9600);
	//uart2_init(115200);
	 
	OLED_Init();
	KEY_Init();
	 
	 //���������ʱ����ʼ����ʹ�ܵȵȳ�ʼ��
	PWM_Init();
	PWMx_Init();
	HGPIO_Init();
	HGPIOx_Init();
	delay_init();
	Set_x(0);
	Set_y(0);
	 
	 
	//���ڣ���ɫ��A10
	OLED_ShowString(1,5,"NiHao!");
	 
//	printf("begin!\r\n");
	int middle,key= 0;
	
	while(1)
	{	
		int key = KEY_Num();
		if(key == 1)
		{
			key_num++;
		}
		else if(key == 2)
		{
			OLED_ShowString(4,1,"gogo");
			break;
		}
		OLED_ShowNum(2,1,key_num,1);
	}
	
	Send_Data(key_num);
	
	if(key_num==0)
	{
		while(1)
		{
			OLED_ShowNum(3,1,flag,1); 
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 1)
			{
				delay_ms(20);
				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 1);	
				delay_ms(20);
				Send_Data(flag);
			}
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 1)
			{
				delay_ms(20);
				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==1);	
				delay_ms(20);
				flag++;
			}
		}
	}
	
	int now_x,now_y,set_x,set_y;
		
		while(1)
		{
			get_data();
			if(x!=0 && y!=0) Contol_Algorithm(x,y,set_d_x,set_d_y);
			
			
//			printf("x = %d  y = %d set_d_x = %d  set_d_y = %d\r\n",x,y,set_d_x,set_d_y);
			//���յ�λ����Ϣ��ʼ����
//			if (USART_RX_STA & 0x8000)  //���յ�����ͷ����
//			{
//				if (USART_RX_BUF[1] == 0 && USART_RX_BUF[2] == 0)  //û�ҵ�С�򣬻�ƽ��״̬
//				{
//					//ֹͣ
//				}
//				else  //���յ���ʼ����
//				{
//					now_x = USART_RX_BUF[1] * 100 + USART_RX_BUF[2];
//					now_y = USART_RX_BUF[3] * 100 + USART_RX_BUF[4];
//					set_x = USART_RX_BUF[5] * 100 + USART_RX_BUF[6];
//					set_y = USART_RX_BUF[7] * 100 + USART_RX_BUF[8];
//					printf("now_x:%d    now_y:%d    set_x:%d    set_y:%d\r\n",now_x,now_y,set_x,set_y);
//					
//					Contol_Algorithm(now_x,now_y,set_x,set_y);
//				}
//				USART_RX_STA = 0;  //������һ�ν���
//			}
		}

}
 
 
 
 
 
 