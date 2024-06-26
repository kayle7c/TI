#include "stm32f10x.h"
#include "string.h"

#include "delay.h"
#include "usart.h"
#include "led.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#define ARRAY_MAX 200
#define POWER_TH  50	//电源开关的晃动角度阈值
#define eps  0.5 //认为两个数相等的精度条件

/*
mpu6050
PB7 		SCL
PB9 		SDA	

蓝牙
PA9     RX
PA10    TX
*/
int power = 0;	// 电源开关，初始化关
int mode = 1;		// 选题

typedef struct Node
{
	float pitch, roll, yaw;  		//欧拉角   加速度pitch对应y轴
	u8 flag;	// 表示用哪个脚来看，1 + 2 + 4 二进制来存
}DATA;


struct pot{
	int x,y;
};

DATA square_side[7]={
{0,0,0,1+2},//pich  , roll        //a
{177,95,0,2}, 									//b
{177,-174,0,2},									//c
{0,-87,0,1+2},  										//d   1,-86
{-94,0,0,1},   										//e
{89,0,0,1}    										//f   92
};	// 打表出来

DATA angle[ARRAY_MAX];	// 角度数据
DATA anglerr[ARRAY_MAX];	// 角度偏差数据
DATA acc[ARRAY_MAX];	// 加速度数据
int hh = 0;		// 队列标识，采用覆盖
int arry_cnt = 0; //数组中的数据多少

int powtim = 0;	// 统计循环次数，有一个大概的时间基准

u8 begin_flag = 0;	// 是否刚开始标记，在开始时更新path0和roll0
float lastpitch = 0, lastroll = 0;

const char Str[6] = {'A','B','C','D','E','F'};	
struct pot des[6][4][4];
int bord_pos[4][4];
float pitch1,roll1,yaw1; 		//欧拉角

int xstep = 0, ystep = 0;

int mode5selt = 0;

int test_state(int *x, int *y);
int is_in_th(float data, int th);
int match_square_side(float angx, float angy, float angz);
void mode2(void);
void mode3(void);
void mode4(void);
void mode5(void);
void mode5_(void);
void mode5_2(void);

void init(void);
float my_abs(float x);
int cmp1(float a,float b);

	
int main(void)
{
	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		  //加速度传感器原始数据
	int powflag11 = 0, powflag12 = 0, powflag21 = 0, powflag22 = 0,powflag3 = 0;	// 标记是否是到左右边界判断电源开关
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	uart_init(9600);
	printf("hhhhh!\r\n");
	//初始化MPU6050
	MPU_Init();					       
	mpu_dmp_init();
	while(mpu_dmp_init()){
		printf("mpu_dmp_init error!\r\n");		
	}    
	printf("mpu_dmp_init success!\r\n");
	
	led_init();
	
	while (1)
	{
		powtim++;
		// 测量
		if(mpu_dmp_get_data(&pitch,&roll,&yaw) != 0)	continue;	// 测量失败
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		
//		if (begin_flag == 1)	// 第一次，更新初始角度
//		{
//			lastpitch = pitch;
//			lastroll = roll;cvv
// 			printf("init pitch0=%.2f   roll0=%.2f\r\n", lastpitch, lastroll);
//			begin_flag = 0;

//		}
//		printf("Pitch:  %f    Roll:  %f    YAW:   %f\r\n",(float)pitch, (float)roll,(float)yaw);
//	  printf("Roll:  %f\r\n",(float)roll);
//		continue;
		
		//printf("TIME : %d\r\n",powtim);
		// 电源开关判断
	  // 若不要干扰的话，加上if(mode == 1)
		if (mode == 1)
		{
			if (powtim > 3000) // 判断超时
			{
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powtim = 0;
			}
			if (powflag11 == 0 && powflag12 == 0 && powflag21 == 0 && powflag22 == 0 && (pitch < -POWER_TH || roll < -POWER_TH || pitch > POWER_TH || roll > POWER_TH))	// 开始计时检测
				powtim = 0;
				
			if (pitch < -POWER_TH)
				powflag11++;
			if (pitch > POWER_TH)
				powflag12++;
			if (roll < -POWER_TH)
				powflag21++;
			if (roll > POWER_TH)
				powflag22++;
			
			//printf("time : %d 	flag1 = %d 	flag2 = %d\r\n",powtim,powflag1,powflag2);
			if ((powflag11 >= 20 && powflag12 >= 20) || (powflag21 >= 20 && powflag22 >= 20))	// 晃动
			{
				power = !power;  // 反转开关
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powflag3 = 1;	// 标记开关操作了
				powtim = 0;
			}
			if (power == 0)
			{
				if (powflag3)	
				{
					printf("power off!\r\n");
					// printf("%c%c\r\n", (u8)1, (u8)0);
					powflag11 = 0;
					powflag12 = 0;
					powflag21 = 0;
					powflag22 = 0;
					powflag3 = 0;
				}
				continue;
			}
			if (powflag3)	
			{
				printf("power on!\r\n");
				// printf("%c%c\r\n", (u8)1, (u8)1);
				// 初始化数据
				hh = 0;
				arry_cnt = 0;
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powflag3 = 0;
			}
		}
		
		// 开关打开，处理数据
		angle[hh].pitch = pitch;
		angle[hh].roll = roll;
		angle[hh].yaw = yaw;	
		anglerr[hh].pitch = angle[hh].pitch - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].pitch;  // 注意第一组数据的角速度还要处理
		anglerr[hh].roll = angle[hh].roll - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].roll;  
		anglerr[hh].yaw = angle[hh].yaw - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].yaw;  
		hh ++;
		hh = hh % ARRAY_MAX;
		if (arry_cnt < ARRAY_MAX) arry_cnt++;
		// 平移存加速度还是算出来的速度这里再进行处理
		// .......
		
		if (arry_cnt < ARRAY_MAX) // 等待数组满才开始测量，有一个稳定过程
		{
		//	printf("wait...\r\n");
			continue;
		}
			
		switch (mode)
		{
			case 2:	// 测量角度
				mode2();
				break;
			case 3:	// 测量顶面标记
				mode3();
				break;
			case 4:
				mode4();	// 翻转
				break;
			case 5:
				if (mode5selt == 1)  // 自动测
					mode5_();
				else  // 手动测
					mode5_2();
				break;
		}
		
	}
	
	
}

// 0静止，1运动
int test_state(int *x, int *y)	// 两个方向的速度方向，在运动时可以使用该数据进行检测
{
	int i;
	float sumaerr1 = 0, sumaerr2 = 0, sumaerr3 = 0;
	int static_err_th = 10;
	int k = 0;	//记录有几个轴静止
	
	for (i = 0; i < ARRAY_MAX; i++)
	{
		sumaerr1 += anglerr[i].pitch;
		sumaerr2 += anglerr[i].roll;
		sumaerr3 += anglerr[i].yaw;
	}
	
	k = is_in_th(sumaerr1, static_err_th) + is_in_th(sumaerr2, static_err_th) + is_in_th(sumaerr3, static_err_th);
	if (k >= 2)	// 有两个轴静止，则表示静止
		return 0;
	
	if (x != NULL && y != NULL)  // 有参数才带回
	{
		*x = sumaerr1 > 0;
		*y = sumaerr2 > 0;
	}
	return 1;
}

// 判断是否在阈值里面
int is_in_th(float data, int th)
{
	if (data > -th && data < th) return 1;
	return 0;
}


// 匹配是哪一个面，0表示匹配的
int match_square_side(float angx, float angy, float angz)
{
	int i;
	int match_th = 7;  // 匹配的阈值
	int flag = 0;
	
	for (i = 0; i < 6; i++)
	{
		flag = square_side[i].flag;
		
		if (flag % 2 == 1 && (!is_in_th(square_side[i].pitch - angx, match_th)))
		{
			if ((my_abs(square_side[i].pitch) > 173 && is_in_th(-square_side[i].pitch - angx, match_th)))
				;
			else continue;
		}
		flag /= 2;
		if (flag % 4 == 1 && (!is_in_th(square_side[i].roll - angy, match_th)))
		{
			if ((my_abs(square_side[i].roll) > 173 && is_in_th(-square_side[i].roll - angy, match_th)))
				;
			else continue;
		}
		flag /= 4;
		if (flag % 8 == 1 && (!is_in_th(square_side[i].yaw - angz, match_th)))
		{
			if ((my_abs(square_side[i].yaw) > 173 && is_in_th(-square_side[i].yaw - angz, match_th)))
				;
			else continue;
		}
		
//		if (flag % 2 == 1 && (!is_in_th(square_side[i].pitch - angx, match_th) && (my_abs(square_side[i].pitch) > 173 && !is_in_th(-square_side[i].pitch - angx, match_th))))	continue;	//测pitch不准
//		flag /= 2;
//		if (flag % 4 == 1 && (!is_in_th(square_side[i].roll - angy, match_th)  && (my_abs(square_side[i].roll) > 173 && !is_in_th(-square_side[i].roll - angy, match_th))))	continue;	//测roll不准
//		flag /= 4;
//		if (flag % 8 == 1 && (!is_in_th(square_side[i].yaw - angz, match_th)  && (my_abs(square_side[i].yaw) > 173 && !is_in_th(-square_side[i].yaw - angz, match_th))))	continue;	//测yaw不准
//		
		return i + 1;
	}
	return 0;
}


// 测量角度 缺点是会一直输出数据
void mode2(void)
{
	float angx = 0, angy = 0;	//沿对应轴翻转的角度
	// 判断状态，静止的时候才进行测量
	if (test_state(NULL, NULL) != 0)	// 运动不测
	{
		return;
	}
	
	// 测量，统计近3次的平均值  
	angy = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].pitch) / 3.0;
	angx = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].roll) / 3.0;
	
	printf("x:%.2f   y:%.2f\r\n", angx, angy);
	
	//	u8 d1, d2, d3, d4;
//	d1 = angx;
//	d2 = (angx - d1) * 100;
//	d3 = angy;
//	d4 = (angy - d2) * 100;
//	printf("%c%c%c%c%c\r\n", (u8)2, d1, d2, d3, d4);
}

void mode3(void)
{
	float angx = 0, angy = 0, angz = 0;
	int side = 0;
	// 判断状态，静止的时候才进行测量
	if (test_state(NULL, NULL) != 0)	// 运动不测
	{
		return;
	}
	angx = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].pitch) / 3.0;
	angy = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].roll) / 3.0;
	angz = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].yaw + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].yaw + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].yaw) / 3.0;
	
	
	if ((side = match_square_side(angx, angy, angz)) == 0)	// 没检测到
	{
		printf("no match on, please check!\r\n");
		printf("tip:x:%.2f   y:%.2f   z:%.2f\r\n", angx, angy, angz);	//提示
		return;
	}
	
	printf("square side is %d\r\n", side);
	//	printf("%c%c\r\n", (u8)3, side);
}





void  mode4(void)
{
				//int res[20]=-1;
//		printf("mode4\r\n");
//		MPU_Init();					       //初始化MPU6050
//	while(mpu_dmp_init()){
//		printf("error	\r\n");
//	}    
//		while(1)	{
//			if(!mpu_dmp_get_data(&pitch,&roll,&yaw))
//			printf("yaw:%.2lf\r\n",yaw);
//			if(re_choose() == 1)	return;	
//		}
		int i=0,j=0,num=0;
		float pitch_error=0,roll_error=0,yam_error=0;	
		float pre_pitch=0,pre_roll=0,pre_yam=0;
		float max=-1,min=361;
		int times=0,p;
		struct pot pre_pot={0,0},cur_pot={0,0};
		//下上左右
		const int dx[4]={0,0,+1,-1};
		const int dy[4]={-1,+1,0,0};
		
		struct pot cur_pos={0,0};
		init();
	
		while(1){
				if(mode != 4)	return;	// 退出条件
				max=-1,min=361;
				mpu_dmp_get_data(&pitch1,&roll1,&yaw1);
				pitch_error=my_abs(pitch1-pre_pitch);
				if(cmp1(pitch_error,max)>0) max=pitch_error;
				//if(cmp1(pitch_error,min)<0) min=pitch_error;
				roll_error=my_abs(roll1-pre_roll);
				if(cmp1(roll_error,max)>0) max=roll_error;
				//if(cmp1(roll_error,min)<0) min=roll_error;
				yam_error=my_abs(yaw1-pre_yam);
				if(cmp1(yam_error,max)>0) max=yam_error;
				//if(cmp1(yam_error,min)<0) min=yam_error;
			  pre_pitch=pitch1;
				pre_roll=roll1;
				pre_yam=yaw1;
				
				//停止条件
				if(max>=5)	times=0;
				else times++;//
				//printf("pre_pot %d\r\n",pre_pot.x);
				//printf("times: %d\r\n",times);
				
				if(times>=50){
					times=0;
						//求解上面的面且设为cur_pot;
						p=match_square_side(pitch1, roll1, yaw1);
					
					//四个方向，下上左右
						if(pre_pot.x==p) continue;
					  //while(1) {
							//printf("pre_pot_x pre_pot_y peak:%d %d %d\r\n",pre_pot.x,pre_pot.y,p);
						//}
						for(i=0;i<4;i++)
							if(des[pre_pot.x][pre_pot.y][i].x==p){
									cur_pot.x=p;
									cur_pot.y=des[pre_pot.x][pre_pot.y][i].y;
									
									cur_pos.x+=dx[i];
									cur_pos.y+=dy[i];
								//printf("cur_pot_x: %d cur_pot_y :%d p: %d pos: %d frow: %d\r\n",cur_pot.x,cur_pot.y,p,bord_pos[cur_pos.x][cur_pos.y],i);
								printf("pos: %d\r\n",bord_pos[cur_pos.x][cur_pos.y]);
								break;
							}
						pre_pot=cur_pot;
				}//认为其已停止运动
		}
}


void init(void){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			bord_pos[i][j]=i*4+j;
	
	//
	des[0][0][0].x=5;
	des[0][0][0].y=1;
	des[0][0][1].x=4;
	des[0][0][1].y=0;
	des[0][0][2].x=1;
	des[0][0][2].y=3;
	des[0][0][3].x=3;
	des[0][0][3].y=2;
	des[0][1][0].x=4;
	des[0][1][0].y=1;
	des[0][1][1].x=5;
	des[0][1][1].y=0;
	des[0][1][2].x=3;
	des[0][1][2].y=3;	
	des[0][1][3].x=1;
	des[0][1][3].y=2;
	des[0][2][0].x=3;
	des[0][2][0].y=1;
	des[0][2][1].x=1;
	des[0][2][1].y=0;	
	des[0][2][2].x=5;
	des[0][2][2].y=3;
	des[0][2][3].x=4;
	des[0][2][3].y=2;
	des[0][3][0].x=1;
	des[0][3][0].y=1;	
	des[0][3][1].x=3;
	des[0][3][1].y=0;
	des[0][3][2].x=4;
	des[0][3][2].y=3;
	des[0][3][3].x=5;
	des[0][3][3].y=2;	
	des[1][0][0].x=0;
	des[1][0][0].y=2;
	des[1][0][1].x=2;
	des[1][0][1].y=2;
	des[1][0][2].x=5;
	des[1][0][2].y=0;	
	des[1][0][3].x=4;
	des[1][0][3].y=0;
	des[1][1][0].x=2;
	des[1][1][0].y=1;
	des[1][1][1].x=0;
	des[1][1][1].y=3;	
	des[1][1][2].x=4;
	des[1][1][2].y=1;
	des[1][1][3].x=5;
	des[1][1][3].y=1;
	des[1][2][0].x=4;
	des[1][2][0].y=2;	
	des[1][2][1].x=5;
	des[1][2][1].y=2;
	des[1][2][2].x=0;
	des[1][2][2].y=1;
	des[1][2][3].x=2;
	des[1][2][3].y=1;	
	des[1][3][0].x=5;
	des[1][3][0].y=3;
	des[1][3][1].x=4;
	des[1][3][1].y=3;
	des[1][3][2].x=2;
	des[1][3][2].y=0;	
	des[1][3][3].x=0;
	des[1][3][3].y=0;
	des[2][0][0].x=5;
	des[2][0][0].y=0;
	des[2][0][1].x=4;
	des[2][0][1].y=1;	
	des[2][0][2].x=3;
	des[2][0][2].y=2;
	des[2][0][3].x=1;
	des[2][0][3].y=3;
	des[2][1][0].x=4;
	des[2][1][0].y=0;
	des[2][1][1].x=5;
  des[2][1][1].y=1;
	des[2][1][2].x=1;
  des[2][1][2].y=2;
	des[2][1][3].x=3;
	des[2][1][3].y=3;
	des[2][2][0].x=1;
	des[2][2][0].y=0;
	des[2][2][1].x=3;
	des[2][2][1].y=1;
	des[2][2][2].x=5;
	des[2][2][2].y=2;	
	des[2][2][3].x=4;
	des[2][2][3].y=3;
	des[2][3][0].x=5;
	des[2][3][0].y=2;
	des[2][3][1].x=1;
	des[2][3][1].y=1;	
	des[2][3][2].x=4;
	des[2][3][2].y=2;
	des[2][3][3].x=5;
	des[2][3][3].y=3;
	des[3][0][0].x=0;
	des[3][0][0].y=3;	
	des[3][0][1].x=2;
	des[3][0][1].y=3;
	des[3][0][2].x=4;
	des[3][0][2].y=0;
	des[3][0][3].x=5;
	des[3][0][3].y=0;	
	des[3][1][0].x=2;
	des[3][1][0].y=2;
	des[3][1][1].x=0;
	des[3][1][1].y=2;
	des[3][1][2].x=5;
	des[3][1][2].y=1;	
	des[3][1][3].x=4;
	des[3][1][3].y=1;
	des[3][2][0].x=5;
	des[3][2][0].y=2;
	des[3][2][1].x=4;
des[3][2][1].y=2;		
	des[3][2][2].x=0;
  des[3][2][2].y=0;
	des[3][2][3].x=2;
des[3][2][3].y=0;			
	des[3][3][0].x=4;
des[3][3][0].y=3;
	des[3][3][1].x=5;
des[3][3][1].y=3;
	des[3][3][2].x=2;
des[3][3][2].y=1;
	des[3][3][3].x=0;
des[3][3][3].y=1;		
	des[4][0][0].x=0;
des[4][0][0].y=0;
	des[4][0][1].x=2;
des[4][0][1].y=1;		
	des[4][0][2].x=1;
des[4][0][2].y=0;
	des[4][0][3].x=3;
des[4][0][3].y=0;		
	des[4][1][0].x=2;
des[4][1][0].y=0;
	des[4][1][1].x=0;
des[4][1][1].y=1;		
	des[4][1][2].x=3;
des[4][1][2].y=1;
	des[4][1][3].x=1;
des[4][1][3].y=1;		
	des[4][2][0].x=3;
des[4][2][0].y=2;
	des[4][2][1].x=1;
des[4][2][1].y=2;		
	des[4][2][2].x=0;
des[4][2][2].y=2;
	des[4][2][3].x=2;
des[4][2][3].y=3;			
	des[4][3][0].x=1;
des[4][3][0].y=3;
	des[4][3][1].x=3;
des[4][3][1].y=3;			
	des[4][3][2].x=2;
des[4][3][2].y=2;
	des[4][3][3].x=0;
des[4][3][3].y=3;		
	des[5][0][0].x=0;
des[5][0][0].y=1;
	des[5][0][1].x=2;	
des[5][0][1].y=0;	
	des[5][0][2].x=3;
des[5][0][2].y=0;
	des[5][0][3].x=1;	
des[5][0][3].y=0;	
	des[5][1][0].x=2;
des[5][1][0].y=1;
	des[5][1][1].x=0;	
des[5][1][1].y=0;	
	des[5][1][2].x=1;
des[5][1][2].y=1;
	des[5][1][3].x=3;
des[5][1][3].y=1;	
	des[5][2][0].x=1;
des[5][2][0].y=2;
	des[5][2][1].x=3;	
des[5][2][1].y=2;
	des[5][2][2].x=0;
des[5][2][2].y=3;
	des[5][2][3].x=2;	
des[5][2][3].y=2;
	des[5][3][0].x=3;
des[5][3][0].y=3;
	des[5][3][1].x=1;	
	des[5][3][1].y=3;
	des[5][3][2].x=2;
	des[5][3][2].y=3;
	des[5][3][3].x=0;	
	des[5][3][3].y=2;		
	
}

float my_abs(float x){
	if(x<0) return -x;
	return x;
}

int cmp1(float a,float b){
		if(a-b>-eps && a-b<eps)	return 0;
		else if(a>b)  return 1;
		else return -1;
}


















short x0 = 0, y0 = 0;	// 两个轴的0点位置
// 上-x   下+x   左-y   右+y
void mode5_(void)
{
	static int x_i = 0, y_i = 0;
	static int ximax = 0, ximin = 0, yimax = 0, yimin = 0;
	int xdir = 0, ydir = 0;
	short aacx,aacy,aacz;		  //加速度传感器原始数据
//	short x0 = 700, y0 = 400;	// 两个轴的0点位置
	int thx = 38000, thy = 45000;
	int th_static = 200;
	int begin_x = 0, begin_y = 0, begin_point = -1, end_point = -1;
	int i, j;

	
	MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	//printf("aacx:%d  aacy:%d\r\n", aacx, aacy);
	if (aacx - x0 >=  -1* th_static && aacx -x0 <= th_static && aacy - y0 >=  -1* th_static && aacy -y0 <= th_static)	// 静止
	{
		//printf("static,begin look!\r\n");
		// 做处理输出
		xdir = ximax + ximin;	// 相加会凸显出原有的方向
		ydir = yimax + yimin;
		
	//printf("-----  %d %d\r\n", xdir, ydir);
		if (xdir > thx || xdir < -1 * thx)	// 前一段过程有突变
		{
			if (xdir < 0) xstep--;
			else xstep++;
			
			//printf("xstep:%d\r\n", xstep);
			if (xdir < 0)	printf("up\r\n");
			else printf("down\r\n");

			
			if (xstep > 0 && ystep > 0)	// 右下 03
			{
				end_point = 3;
				begin_x = 0 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep > 0 && ystep < 0)	// 左下 00
			{
				end_point = 0;
				begin_x = 0 + xstep;
				begin_y = 0 - ystep;
			}
			if (xstep < 0 && ystep > 0)	// 右上 15  33
			{
				end_point = 15;
				begin_x = 3 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep < 0 && ystep < 0)	// 左上 12   30
			{
				end_point = 12;
				begin_x = 3 + xstep;
				begin_y = 0 - ystep;
			}
			
			begin_point = begin_x * 4 + begin_y;
			printf("end_point:%d\r\n", end_point);
			printf("begin_point:%d    %d %d\r\n\r\n", begin_point, begin_x, begin_y);
			
	//		printf("xdir:%d\r\n\r\n", xdir);
			
			
			for (i = 0; i < 72000; i++)
				for (j = 0; j < 100; j++);
			
		}
		if (ydir > thy || ydir < -1 * thy)	// 前一段过程有突变
		{
			if (ydir < 0) ystep--;
			else ystep++;
			
	//		printf("ystep:%d\r\n", ystep);
			
			if (ydir < 0)	printf("left\r\n");
			else printf("right\r\n");
			
			if (xstep > 0 && ystep > 0)	// 右下 03
			{
				end_point = 3;
				begin_x = 0 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep > 0 && ystep < 0)	// 左下 00
			{
				end_point = 0;
				begin_x = 0 + xstep;
				begin_y = 0 + ystep;
			}
			if (xstep < 0 && ystep > 0)	// 右上 15  33
			{
				end_point = 15;
				begin_x = 3 - xstep;
				begin_y = 3 - ystep;
			}
			if (xstep < 0 && ystep < 0)	// 左上 12   30
			{
				end_point = 12;
				begin_x = 3 - xstep;
				begin_y = 0 + ystep;
			}
			
			begin_point = begin_x * 3 + begin_y;
			printf("end_point:%d\r\n", end_point);
			printf("begin_point:%d    %d %d\r\n\r\n", begin_point, begin_x, begin_y);
//			printf("ydir:%d\r\n\r\n", ydir);
			
			for (i = 0; i < 72000; i++)
				for (j = 0; j < 100; j++);
		}
		
		//printf("-----------------------------clear!\r\n");
		x_i = 0, y_i = 0;
		ximax = 0, ximin = 0, yimax = 0, yimin = 0;
	}
	
	//MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	aacx -= x0;
	aacy -= y0;
	x_i += aacx;
	y_i += aacy;
	//printf("xi:%d   yi:%d\r\n", x_i, y_i);
	if (ximax < x_i) ximax = x_i;
	if (ximin >  x_i) ximin = x_i;
	if (yimax < y_i) yimax = y_i;
	if (yimin >  y_i) yimin = y_i;
	//printf("ximax:%d   ximin:%d   yimax:%d   yimin:%d\r\n", ximax, ximin, yimax, yimin);
	

}


int end_point = -1, end_x = -1, end_y = -1;

void mode5_2(void)
{
	static int x_i = 0, y_i = 0;
	static int ximax = 0, ximin = 0, yimax = 0, yimin = 0;
	int xdir = 0, ydir = 0;
	short aacx,aacy,aacz;		  //加速度传感器原始数据
//	short x0 = 700, y0 = 400;	// 两个轴的0点位置
	int thx = 37000, thy = 45000;
	int th_static = 200;
	int begin_point = -1, begin_x = -1, begin_y = -1;
	int i, j;

	
	MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	//printf("aacx:%d  aacy:%d\r\n", aacx, aacy);
	if (aacx - x0 >=  -1* th_static && aacx -x0 <= th_static && aacy - y0 >=  -1* th_static && aacy -y0 <= th_static)	// 静止
	{
		//printf("static,begin look!\r\n");
		// 做处理输出
		xdir = ximax + ximin;	// 相加会凸显出原有的方向
		ydir = yimax + yimin;
		
	//printf("-----  %d %d\r\n", xdir, ydir);
		if (xdir > thx || xdir < -1 * thx)	// 前一段过程有突变
		{
			if (xdir < 0) xstep--;
			else xstep++;
			
			begin_x = end_x + xstep;
			begin_y = end_y - ystep;
			begin_point = begin_x * 4 + begin_y;
			
			printf("begin_point:%d    %d   %d\r\n\r\n", begin_point, begin_x, begin_y);
			
		//	LED = 0;
			for (i = 0; i < 72000; i++)
				for (j = 0; j < 100; j++);
		//	LED = 1;
		}
		if (ydir > thy || ydir < -1 * thy)	// 前一段过程有突变
		{
			if (ydir < 0) ystep--;
			else ystep++;
			
			begin_x = end_x + xstep;
			begin_y = end_y - ystep;
			begin_point = begin_x * 4 + begin_y;
			
			printf("begin_point:%d\r\n\r\n", begin_point);
			
//			printf("ydir:%d\r\n\r\n", ydir);
		//	LED = 0;
			for (i = 0; i < 72000; i++)
				for (j = 0; j < 100; j++);
		//	LED = 1;
		}
		
		//printf("-----------------------------clear!\r\n");
		x_i = 0, y_i = 0;
		ximax = 0, ximin = 0, yimax = 0, yimin = 0;
	}
	
	//MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	aacx -= x0;
	aacy -= y0;
	x_i += aacx;
	y_i += aacy;
	//printf("xi:%d   yi:%d\r\n", x_i, y_i);
	if (ximax < x_i) ximax = x_i;
	if (ximin >  x_i) ximin = x_i;
	if (yimax < y_i) yimax = y_i;
	if (yimin >  y_i) yimin = y_i;
	//printf("ximax:%d   ximin:%d   yimax:%d   yimin:%d\r\n", ximax, ximin, yimax, yimin);
	

}





