#include "bsp_usart.h"


void Usart_SendByte(USART_TypeDef*pUSARTx,uint8_t data)  //串口发送8位的数据到蓝牙模块
{
	USART_SendData(pUSARTx,data);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
}


