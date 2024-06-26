#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bluetooth.h"

void BL_Send(USART_TypeDef*pUSARTx,u16 pwmx,u16 pwmy);  //发送数据给车


#endif 

