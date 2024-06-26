#include "control.h" 
#include "math.h"
#include "oled.h"


static float X_Position_KP = 0; //x->p    
static float X_Position_KI = 0; //x->i
static float X_Position_KD = 0; //x->d

static float X_v_KP = 0; //x->p    
static float X_v_KI = 0; //x->i
static float X_v_KD = 0; //x->d

int last_x_bias = 0; //��һ�ε����
static int Integral_x_bias = 0; //�����޷�
static int limit_v = 10; //����޷�
int flag=1;

int last_v_bias = 0; //��һ�ε����
static int Integral_v_bias = 0; //�����޷�
static int limit_a = 10; //����޷�


void PID_Control_v(int position_x, int aim_x)
{
	int x_bias=0; //��ǰλ����Ŀ��λ�õ�ƫ��ֵ
	int x_bias_v=0;
	int x_angle=0; //�Ƕ�
	if(position_x<=aim_x)
	{
		x_bias = aim_x-position_x;
		flag=-1;
	}
	else 
	{
		x_bias=position_x-aim_x;
		flag=1;
	}		// ����ƫ��ֵ
//******************************************
//���ֲ��� i
	if(fabs(x_bias) < 10) 
		Integral_x_bias = 0;
	
	Integral_x_bias += x_bias;
//*******************************************
//	if(bFirst_pid)//��һ��
//	{
//		x_angle = X_Position_KP * x_bias;  // ��һ��ʱ����last_bias
//		bFirst_pid = 0;
//	}
//	else//����һ��֮��
//	{
	if(x_bias>=35)
	{
			X_Position_KP = 4.5;    
			X_Position_KI = 0.01;  
			X_Position_KD = 80;  
	}
	else if(x_bias>=25)
	{
		X_Position_KP = 0.08;    
		X_Position_KI = 0;  
		X_Position_KD = 120;
	}
	else
	{
		X_Position_KP = 0.048;    
		X_Position_KI = 0;  
		X_Position_KD = 150;
		
	}
		x_angle = X_Position_KP*x_bias + X_Position_KI*Integral_x_bias + X_Position_KD*(x_bias-last_x_bias);  
	//}
	OLED_ShowNum(2,1,x_bias,5);
	OLED_ShowNum(3,1,x_angle,5);
	last_x_bias = x_bias;            // ������һ��ƫ��
	
	x_angle = limit_angle_v(x_angle,flag);  // �޷���ת������ 85-95
	x_bias_v=x_bias-last_x_bias;
	//printf("%f%f",x_angle,y_angle);
	PID_Control_a(x_angle,x_bias_v);
}

void PID_Control_a(int position_v, int aim_v)
{
	int x_bias=0; //��ǰλ����Ŀ��λ�õ�ƫ��ֵ
	int x_angle=0; //�Ƕ�
	if(position_v<=aim_v)
	{
		x_bias = aim_v-position_v;
		flag=-1;
	}
	else 
	{
		x_bias=position_v-aim_v;
		flag=1;
	}		// ����ƫ��ֵ
//******************************************
//���ֲ��� i
	if(fabs(x_bias) < 10) 
		Integral_x_bias = 0;
	
	Integral_x_bias += x_bias;
//******************************************

			X_Position_KP = 4.5;    
			X_Position_KI = 0.01;  
			X_Position_KD = 80;  
	
		x_angle = X_Position_KP*x_bias + X_Position_KI*Integral_x_bias + X_Position_KD*(x_bias-last_x_bias);  
	
	last_x_bias = x_bias;            // ������һ��ƫ��
	
	x_angle = limit_angle_a(x_angle,flag);  // �޷���ת������ 85-95
	//printf("%f%f",x_angle,y_angle);
	
	CTRL_X(x_angle);
	
}


void Control_Reset(void)
{
	CTRL_X(90);
	
	last_x_bias = 0;
	Integral_x_bias = 0;
}
	

int limit_angle_v(int angle,int flag)
{
	if(angle <= -limit_v)
		angle = -limit_v;
	if(angle >= limit_v)
		angle = limit_v;
	if(flag==1) angle = 90 - angle;
	else angle = 90 + angle;
	
	return angle;
}

int limit_angle_a(int angle,int flag)
{
	if(angle <= -limit_v)
		angle = -limit_v;
	if(angle >= limit_v)
		angle = limit_v;
	if(flag==1) angle = 90 - angle;
	else angle = 90 + angle;
	
	return angle;
}





