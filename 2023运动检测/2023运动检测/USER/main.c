#include "stm32f10x.h"
#include "string.h"

#include "delay.h"
#include "usart.h"
#include "led.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#define ARRAY_MAX 200
#define POWER_TH  50	//��Դ���صĻζ��Ƕ���ֵ
#define eps  0.5 //��Ϊ��������ȵľ�������

/*
mpu6050
PB7 		SCL
PB9 		SDA	

����
PA9     RX
PA10    TX
*/
int power = 0;	// ��Դ���أ���ʼ����
int mode = 1;		// ѡ��

typedef struct Node
{
	float pitch, roll, yaw;  		//ŷ����   ���ٶ�pitch��Ӧy��
	u8 flag;	// ��ʾ���ĸ���������1 + 2 + 4 ����������
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
};	// ������

DATA angle[ARRAY_MAX];	// �Ƕ�����
DATA anglerr[ARRAY_MAX];	// �Ƕ�ƫ������
DATA acc[ARRAY_MAX];	// ���ٶ�����
int hh = 0;		// ���б�ʶ�����ø���
int arry_cnt = 0; //�����е����ݶ���

int powtim = 0;	// ͳ��ѭ����������һ����ŵ�ʱ���׼

u8 begin_flag = 0;	// �Ƿ�տ�ʼ��ǣ��ڿ�ʼʱ����path0��roll0
float lastpitch = 0, lastroll = 0;

const char Str[6] = {'A','B','C','D','E','F'};	
struct pot des[6][4][4];
int bord_pos[4][4];
float pitch1,roll1,yaw1; 		//ŷ����

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
	float pitch,roll,yaw; 		//ŷ����
	short aacx,aacy,aacz;		  //���ٶȴ�����ԭʼ����
	int powflag11 = 0, powflag12 = 0, powflag21 = 0, powflag22 = 0,powflag3 = 0;	// ����Ƿ��ǵ����ұ߽��жϵ�Դ����
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	uart_init(9600);
	printf("hhhhh!\r\n");
	//��ʼ��MPU6050
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
		// ����
		if(mpu_dmp_get_data(&pitch,&roll,&yaw) != 0)	continue;	// ����ʧ��
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		
//		if (begin_flag == 1)	// ��һ�Σ����³�ʼ�Ƕ�
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
		// ��Դ�����ж�
	  // ����Ҫ���ŵĻ�������if(mode == 1)
		if (mode == 1)
		{
			if (powtim > 3000) // �жϳ�ʱ
			{
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powtim = 0;
			}
			if (powflag11 == 0 && powflag12 == 0 && powflag21 == 0 && powflag22 == 0 && (pitch < -POWER_TH || roll < -POWER_TH || pitch > POWER_TH || roll > POWER_TH))	// ��ʼ��ʱ���
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
			if ((powflag11 >= 20 && powflag12 >= 20) || (powflag21 >= 20 && powflag22 >= 20))	// �ζ�
			{
				power = !power;  // ��ת����
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powflag3 = 1;	// ��ǿ��ز�����
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
				// ��ʼ������
				hh = 0;
				arry_cnt = 0;
				powflag11 = 0;
				powflag12 = 0;
				powflag21 = 0;
				powflag22 = 0;
				powflag3 = 0;
			}
		}
		
		// ���ش򿪣���������
		angle[hh].pitch = pitch;
		angle[hh].roll = roll;
		angle[hh].yaw = yaw;	
		anglerr[hh].pitch = angle[hh].pitch - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].pitch;  // ע���һ�����ݵĽ��ٶȻ�Ҫ����
		anglerr[hh].roll = angle[hh].roll - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].roll;  
		anglerr[hh].yaw = angle[hh].yaw - angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].yaw;  
		hh ++;
		hh = hh % ARRAY_MAX;
		if (arry_cnt < ARRAY_MAX) arry_cnt++;
		// ƽ�ƴ���ٶȻ�����������ٶ������ٽ��д���
		// .......
		
		if (arry_cnt < ARRAY_MAX) // �ȴ��������ſ�ʼ��������һ���ȶ�����
		{
		//	printf("wait...\r\n");
			continue;
		}
			
		switch (mode)
		{
			case 2:	// �����Ƕ�
				mode2();
				break;
			case 3:	// ����������
				mode3();
				break;
			case 4:
				mode4();	// ��ת
				break;
			case 5:
				if (mode5selt == 1)  // �Զ���
					mode5_();
				else  // �ֶ���
					mode5_2();
				break;
		}
		
	}
	
	
}

// 0��ֹ��1�˶�
int test_state(int *x, int *y)	// ����������ٶȷ������˶�ʱ����ʹ�ø����ݽ��м��
{
	int i;
	float sumaerr1 = 0, sumaerr2 = 0, sumaerr3 = 0;
	int static_err_th = 10;
	int k = 0;	//��¼�м����ᾲֹ
	
	for (i = 0; i < ARRAY_MAX; i++)
	{
		sumaerr1 += anglerr[i].pitch;
		sumaerr2 += anglerr[i].roll;
		sumaerr3 += anglerr[i].yaw;
	}
	
	k = is_in_th(sumaerr1, static_err_th) + is_in_th(sumaerr2, static_err_th) + is_in_th(sumaerr3, static_err_th);
	if (k >= 2)	// �������ᾲֹ�����ʾ��ֹ
		return 0;
	
	if (x != NULL && y != NULL)  // �в����Ŵ���
	{
		*x = sumaerr1 > 0;
		*y = sumaerr2 > 0;
	}
	return 1;
}

// �ж��Ƿ�����ֵ����
int is_in_th(float data, int th)
{
	if (data > -th && data < th) return 1;
	return 0;
}


// ƥ������һ���棬0��ʾƥ���
int match_square_side(float angx, float angy, float angz)
{
	int i;
	int match_th = 7;  // ƥ�����ֵ
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
		
//		if (flag % 2 == 1 && (!is_in_th(square_side[i].pitch - angx, match_th) && (my_abs(square_side[i].pitch) > 173 && !is_in_th(-square_side[i].pitch - angx, match_th))))	continue;	//��pitch��׼
//		flag /= 2;
//		if (flag % 4 == 1 && (!is_in_th(square_side[i].roll - angy, match_th)  && (my_abs(square_side[i].roll) > 173 && !is_in_th(-square_side[i].roll - angy, match_th))))	continue;	//��roll��׼
//		flag /= 4;
//		if (flag % 8 == 1 && (!is_in_th(square_side[i].yaw - angz, match_th)  && (my_abs(square_side[i].yaw) > 173 && !is_in_th(-square_side[i].yaw - angz, match_th))))	continue;	//��yaw��׼
//		
		return i + 1;
	}
	return 0;
}


// �����Ƕ� ȱ���ǻ�һֱ�������
void mode2(void)
{
	float angx = 0, angy = 0;	//�ض�Ӧ�ᷭת�ĽǶ�
	// �ж�״̬����ֹ��ʱ��Ž��в���
	if (test_state(NULL, NULL) != 0)	// �˶�����
	{
		return;
	}
	
	// ������ͳ�ƽ�3�ε�ƽ��ֵ  
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
	// �ж�״̬����ֹ��ʱ��Ž��в���
	if (test_state(NULL, NULL) != 0)	// �˶�����
	{
		return;
	}
	angx = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].pitch + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].pitch) / 3.0;
	angy = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].roll + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].roll) / 3.0;
	angz = (angle[(hh - 1 + ARRAY_MAX) % ARRAY_MAX].yaw + angle[(hh - 2 + ARRAY_MAX) % ARRAY_MAX].yaw + angle[(hh - 3 + ARRAY_MAX) % ARRAY_MAX].yaw) / 3.0;
	
	
	if ((side = match_square_side(angx, angy, angz)) == 0)	// û��⵽
	{
		printf("no match on, please check!\r\n");
		printf("tip:x:%.2f   y:%.2f   z:%.2f\r\n", angx, angy, angz);	//��ʾ
		return;
	}
	
	printf("square side is %d\r\n", side);
	//	printf("%c%c\r\n", (u8)3, side);
}





void  mode4(void)
{
				//int res[20]=-1;
//		printf("mode4\r\n");
//		MPU_Init();					       //��ʼ��MPU6050
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
		//��������
		const int dx[4]={0,0,+1,-1};
		const int dy[4]={-1,+1,0,0};
		
		struct pot cur_pos={0,0};
		init();
	
		while(1){
				if(mode != 4)	return;	// �˳�����
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
				
				//ֹͣ����
				if(max>=5)	times=0;
				else times++;//
				//printf("pre_pot %d\r\n",pre_pot.x);
				//printf("times: %d\r\n",times);
				
				if(times>=50){
					times=0;
						//��������������Ϊcur_pot;
						p=match_square_side(pitch1, roll1, yaw1);
					
					//�ĸ�������������
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
				}//��Ϊ����ֹͣ�˶�
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


















short x0 = 0, y0 = 0;	// �������0��λ��
// ��-x   ��+x   ��-y   ��+y
void mode5_(void)
{
	static int x_i = 0, y_i = 0;
	static int ximax = 0, ximin = 0, yimax = 0, yimin = 0;
	int xdir = 0, ydir = 0;
	short aacx,aacy,aacz;		  //���ٶȴ�����ԭʼ����
//	short x0 = 700, y0 = 400;	// �������0��λ��
	int thx = 38000, thy = 45000;
	int th_static = 200;
	int begin_x = 0, begin_y = 0, begin_point = -1, end_point = -1;
	int i, j;

	
	MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	//printf("aacx:%d  aacy:%d\r\n", aacx, aacy);
	if (aacx - x0 >=  -1* th_static && aacx -x0 <= th_static && aacy - y0 >=  -1* th_static && aacy -y0 <= th_static)	// ��ֹ
	{
		//printf("static,begin look!\r\n");
		// ���������
		xdir = ximax + ximin;	// ��ӻ�͹�Գ�ԭ�еķ���
		ydir = yimax + yimin;
		
	//printf("-----  %d %d\r\n", xdir, ydir);
		if (xdir > thx || xdir < -1 * thx)	// ǰһ�ι�����ͻ��
		{
			if (xdir < 0) xstep--;
			else xstep++;
			
			//printf("xstep:%d\r\n", xstep);
			if (xdir < 0)	printf("up\r\n");
			else printf("down\r\n");

			
			if (xstep > 0 && ystep > 0)	// ���� 03
			{
				end_point = 3;
				begin_x = 0 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep > 0 && ystep < 0)	// ���� 00
			{
				end_point = 0;
				begin_x = 0 + xstep;
				begin_y = 0 - ystep;
			}
			if (xstep < 0 && ystep > 0)	// ���� 15  33
			{
				end_point = 15;
				begin_x = 3 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep < 0 && ystep < 0)	// ���� 12   30
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
		if (ydir > thy || ydir < -1 * thy)	// ǰһ�ι�����ͻ��
		{
			if (ydir < 0) ystep--;
			else ystep++;
			
	//		printf("ystep:%d\r\n", ystep);
			
			if (ydir < 0)	printf("left\r\n");
			else printf("right\r\n");
			
			if (xstep > 0 && ystep > 0)	// ���� 03
			{
				end_point = 3;
				begin_x = 0 + xstep;
				begin_y = 3 - ystep;
			}
			if (xstep > 0 && ystep < 0)	// ���� 00
			{
				end_point = 0;
				begin_x = 0 + xstep;
				begin_y = 0 + ystep;
			}
			if (xstep < 0 && ystep > 0)	// ���� 15  33
			{
				end_point = 15;
				begin_x = 3 - xstep;
				begin_y = 3 - ystep;
			}
			if (xstep < 0 && ystep < 0)	// ���� 12   30
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
	short aacx,aacy,aacz;		  //���ٶȴ�����ԭʼ����
//	short x0 = 700, y0 = 400;	// �������0��λ��
	int thx = 37000, thy = 45000;
	int th_static = 200;
	int begin_point = -1, begin_x = -1, begin_y = -1;
	int i, j;

	
	MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	//printf("aacx:%d  aacy:%d\r\n", aacx, aacy);
	if (aacx - x0 >=  -1* th_static && aacx -x0 <= th_static && aacy - y0 >=  -1* th_static && aacy -y0 <= th_static)	// ��ֹ
	{
		//printf("static,begin look!\r\n");
		// ���������
		xdir = ximax + ximin;	// ��ӻ�͹�Գ�ԭ�еķ���
		ydir = yimax + yimin;
		
	//printf("-----  %d %d\r\n", xdir, ydir);
		if (xdir > thx || xdir < -1 * thx)	// ǰһ�ι�����ͻ��
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
		if (ydir > thy || ydir < -1 * thy)	// ǰһ�ι�����ͻ��
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





