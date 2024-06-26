#include "bluetooth.h"
#include "bsp_usart.h"
#include "stm32f10x_it.h" 

void BL_Send(USART_TypeDef*pUSARTx,u16 pitch,u16 roll)//串口发送数据包到蓝牙
{
	
	u8 t;
	u8 sum=0;                    //校验位--数据字节之和的低八位
	Usart_SendByte(pUSARTx,0xA5);//发送头
	
	t=(pitch>>0)&0x00FF;         //16位的pitch分两次发送
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	t=(pitch>>8)&0x00FF;
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	
	
	t=(roll>>0)&0x00FF;          //发送16位的roll
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	t=(roll>>8)&0x00FF;
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	

	Usart_SendByte(pUSARTx,sum);  //发送校验位
	Usart_SendByte(pUSARTx,0x5A); //发送尾
	
}

void led_init(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//先使能外设IO PORTC时钟 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
  //GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);				 //PB10,PB11 输出高	
	
}


