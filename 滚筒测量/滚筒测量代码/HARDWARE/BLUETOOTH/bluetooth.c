#include "bluetooth.h"
#include "bsp_usart.h"
#include "stm32f10x_it.h" 

void BL_Send(USART_TypeDef*pUSARTx,u16 pitch,u16 roll)//���ڷ������ݰ�������
{
	
	u8 t;
	u8 sum=0;                    //У��λ--�����ֽ�֮�͵ĵͰ�λ
	Usart_SendByte(pUSARTx,0xA5);//����ͷ
	
	t=(pitch>>0)&0x00FF;         //16λ��pitch�����η���
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	t=(pitch>>8)&0x00FF;
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	
	
	t=(roll>>0)&0x00FF;          //����16λ��roll
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	t=(roll>>8)&0x00FF;
	sum+=t;
	Usart_SendByte(pUSARTx,t);
	

	Usart_SendByte(pUSARTx,sum);  //����У��λ
	Usart_SendByte(pUSARTx,0x5A); //����β
	
}

void led_init(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//��ʹ������IO PORTCʱ�� 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 // �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 
	
  //GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);				 //PB10,PB11 �����	
	
}


