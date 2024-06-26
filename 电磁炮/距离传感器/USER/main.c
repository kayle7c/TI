#include "sys.h"
#include "delay.h"
#include "OLED.h"
#include "IC.h"
#include "key.h"
#include "sensor.h"
#include "usart.h"

//************************************************************
//   PA.9->TX
//   PA.10->RX
//	
//************************************************************

 int main(void)
 {	
	 delay_init();
	 OLED_Init();
	 int x1,x2,x,y;
	 while(1){
			if(((USART_RX_STA&0x8000)>>15)==1)
			{
				if(USART_RX_BUF[0]==0xA0)
				{
					x1=USART_RX_BUF[1];
					x2=USART_RX_BUF[2];
					y=USART_RX_BUF[3];
					x=x1*100+x2;
					USART_RX_STA=0;
			}
			
		}
	 }
	 
 }
