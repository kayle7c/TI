#include "sys.h"
#include "delay.h"
#include "usart.h"


int x,y,key;


int main(void)
 {	
		delay_init();
		uart_init(9600);
		while(1)
		{
			printf("666\n");
		}
 }
