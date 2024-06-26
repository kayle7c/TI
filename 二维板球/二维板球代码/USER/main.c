#include "sys.h"
#include "delay.h"
#include "usart.h"// RX->9 TX->A10
#include "OLED.h" //B 5 6 7 8 9
#include "pid.h"
#include "PWM.h"	//A 1 2
#include "KEY.h"	//A 3 4
#include "EXIT.h"	
#include "AD.h"
//#include ""

extern u8 USART_RX_BUF[USART_REC_LEN];
extern u16 USART_RX_STA;

//283,224,168,105,37;

int OBJ_SEAT[10][2] = {{0,0},{122,212},{121,145},{118,78},{181,214},{186,147},{181,76},{246,212},{251,130},{253,70}};

int flag1=0;
int time=0;
int x,y=1;
int num3,num4;
extern int temp;
int p;

int set_d_x;
int set_d_y;

 int main(void)
 {	
		uart_init(115200);
		delay_init();
		OLED_Init();
		PWM_init();
	  PID_init();
		KEY_Init();
		while(1)
		{	
			int key = keynum3();
			if(key == 1)
			{
				num3++;
			}
			else if(key == 2)
			{
				OLED_ShowString(1,4,"MODE:");
				break;
			}
			OLED_ShowNum(1,9,num3,1);
		}
			////************************
			while(1)
			{
				switch(num3)
				{
					case 1: TIM_SetCompare2(TIM2,154);
									TIM_SetCompare3(TIM2,152);
					break;
										
					case 2:
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[5][0];
								set_d_y=OBJ_SEAT[5][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
							}
						}
						break;
						
					case 3:
						int cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[4][0];
								set_d_y=OBJ_SEAT[4][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
								if((x<=190||x>=170)&&(y<=220||y>=188)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=250) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[5][0];
								set_d_y=OBJ_SEAT[5][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
							}
						}
						break;
						
					case 4:
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[9][0];
								set_d_y=OBJ_SEAT[9][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
							}
						}
						break;
						
					case 5:
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[2][0];
								set_d_y=OBJ_SEAT[2][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
								if((x<=132||x>=110)&&(y<=150||y>=130)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=50) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[2][0];
								set_d_y=OBJ_SEAT[2][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
								if((x<=173||x>=185)&&(y<=80||y>=60)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=100) break;
							}
						}
						
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[9][0];
								set_d_y=OBJ_SEAT[9][1];
								Contol_Algorithm_x(x);
								Contol_Algorithm_y(y);
							}
						}
						break;
						
					case 6:
						flag1++;
						if(flag1==4)
						{
							while(1)
							{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[9][0];
								set_d_y=OBJ_SEAT[9][1];
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
							}
						}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[2][0];
								set_d_y=OBJ_SEAT[2][1];
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=130||x>=125)&&(y<=133||y>=122)) cnt++; //
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=143;
								set_d_y=92;
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=135||x>=150)&&(y<=83||y>=100)) cnt++; //
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[6][0];
								set_d_y=OBJ_SEAT[6][1];
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=182||x>=172)&&(y<=77||y>=65)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=215;
								set_d_y=97;
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=223||x>=207)&&(y<=88||y>=107)) cnt++; //
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[8][0];
								set_d_y=OBJ_SEAT[8][1];
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=240||x>=230)&&(y<=133||y>=122)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=210;
								set_d_y=162;
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=220||x>=207)&&(y<=170||y>=154)) cnt++; //
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=OBJ_SEAT[4][0];
								set_d_y=OBJ_SEAT[4][1];
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=180||x>=170)&&(y<=190||y>=180)) cnt++;
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
						cnt=0;
						while(1)
						{
							if(((USART_RX_STA&0x8000)>>15)==1)
							{
								x=USART_RX_BUF[1]*100+USART_RX_BUF[2];
								y=USART_RX_BUF[3]*100+USART_RX_BUF[4];
								USART_RX_STA=0; 
								set_d_x=146;
								set_d_y=167;
								Contol_Algorithm_x1(x);
								Contol_Algorithm_y1(y);
								if((x<=155||x>=238)&&(y<=178||y>=159)) cnt++; //
								OLED_ShowNum(2,9,cnt,5);
								if(cnt>=30) break;
							}
						}
					}
				}
			}
