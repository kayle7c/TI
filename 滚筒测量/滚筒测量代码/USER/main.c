#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "OLED.h"
#include "math.h"

//蜂鸣器a4

int x,y;
int num=0;
float pitch,roll,yaw;//欧拉角
float distance=0;
int quan=0;
extern int Num;

float pre_angle;
float cur_angle=180;
float sum=0;
float dif_angle;
float cha;
int   sta;
int b;

int get_dir()
{
	cha=cur_angle-pre_angle;
	if((cha<=0&&cha>=-180)||(cha>=180&&cha<=360)) return 1;
	else return -1;
}

void get_angle(void)
{
	while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0)
			//得到dmp处理后的数据
			//pitch:俯仰角 精度:0.1°   范围:-90.0° <---> +90.0°
			//roll:横滚角  精度:0.1°   范围:-180.0°<---> +180.0°
			//yaw:航向角   精度:0.1°   范围:-180.0°<---> +180.0°
			//返回值:0,正常
			//其他,失败
		{
			
			OLED_ShowString(1,1,"pitch:");
			OLED_ShowString(2,1,"roll :");
			OLED_ShowString(3,1,"yall :");
//*********************************************			
			x=(pitch*10)/10;
			OLED_ShowSignedNum(1,7,x,3);
			OLED_ShowChar(1,11,'.');
			y=(pitch*10);
			y%=1000;
			OLED_ShowNum(1,12,y,1);
//*********************************************				
			x=(roll*10)/10;
			OLED_ShowSignedNum(2,7,x,4);
			OLED_ShowChar(2,12,'.');
			y=(roll*10);
			y%=1000;
			OLED_ShowNum(2,13,y,1);
//*********************************************				
			x=(yaw*10)/10;
			OLED_ShowSignedNum(3,7,x,4);
			OLED_ShowChar(3,12,'.');
			y=(yaw*10);
			y%=1000;
			OLED_ShowNum(3,13,y,1);
//*********************************************				
		}
}

int main(void)
{ 
	int a;
	OLED_Init();
	delay_init();	    //延时函数初始化	  
	key_init();
	MPU_Init();			//初始化MPU6050
	while(mpu_dmp_init()!=0){
		a=mpu_dmp_init();
		OLED_ShowNum(1,1,a,2);
		if(a==0) break;
	} //等待初始化MPU6050完成
	while(1)
		{	
			OLED_ShowString(4,1,"mode:");
			OLED_ShowNum(4,8,num,1);
			int key = key_scan();
			if(key == 1)
			{
				num++;
			}
			else if(key == 2)
			{
				break;
			}
		}
	OLED_Clear();
 	while(1)
	{
		switch(num)
		{
			case 1:
				while(1)
				{
					get_angle();
					if(pitch<=1&&pitch>=-1)
					{
							OLED_ShowString(4,4,"pingheng");
					}
					else
					{
							OLED_ShowString(4,4,"bupingheng");
					}
				}
				break;
			
			
			case 2:
				while(1)
				{
					get_angle();
				}
				break;
				
			case 3:
					while(1)
					{
						pre_angle=cur_angle;
						get_angle();
						cur_angle=roll+180;
						sta=get_dir();	
						if(sta==1)
						{
							if(pre_angle>cur_angle) sum+=pre_angle-cur_angle;
							if(pre_angle<cur_angle) sum+=(360-cur_angle)+pre_angle;
						}else
						{
							if(pre_angle<cur_angle) sum-=cur_angle-pre_angle;
							if(pre_angle>cur_angle) sum-=(360-pre_angle)+cur_angle;		
						}
						distance=sum*0.05758;
						OLED_ShowString(4,1,"dis:");
						OLED_ShowNum(4,5,distance,3);
						OLED_ShowChar(4,8,'.');
						OLED_ShowNum(4,9,(int)(distance*100)%100,2);
					}
					break;

			case 4:
					while(1)
					{
						pre_angle=cur_angle;
						get_angle();
						cur_angle=yaw+180;
						sta=get_dir();
						if(sta==1)
						{
							if(pre_angle>cur_angle) sum+=pre_angle-cur_angle;
							if(pre_angle<cur_angle) sum+=(360-cur_angle)+pre_angle;
						}else
						{
							if(pre_angle<cur_angle) sum-=cur_angle-pre_angle;
							if(pre_angle>cur_angle) sum-=(360-pre_angle)+cur_angle;		
						}
//						if(pre_angle>cur_angle) sum+=pre_angle-cur_angle;
//						if(pre_angle<cur_angle) sum+=(360-cur_angle)+pre_angle;
						quan=sum/360;
						OLED_ShowString(4,1,"quan:");
						OLED_ShowNum(4,6,quan,7);
					}
					break;
			
			case 5:
					Num=0;
					while(1)
					{
					  
						get_angle();
						
					}
			
			
				  break;
		}
		
		
	} 	
}

