#include "bsp_usart.h"


void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data)  //���ڷ���8λ�����ݵ�����ģ��
{
	USART_SendData(pUSARTx,data);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
}


