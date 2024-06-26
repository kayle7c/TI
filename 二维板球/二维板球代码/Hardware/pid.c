#include "pid.h"
#include "usart.h"
#include "pwm.h"
#include "OLED.h"


PID a_pid_x = {10, 0, 0, 0, 0, 0, 0.02, 10, 15};
PID v_pid_x = {0.028, 0, 0, 9, 0, 0, 0.02, 10, 20};

PID a_pid_y = {10, 0, 0, 0, 0, 0, 0.02, 10, 15};
PID v_pid_y = {0.028, 0, 0, 9, 0, 0, 0.02, 10, 20};

PID a_pid_x1 = {7, 0, 0, 0, 0, 0, 0.02, 10, 15};
PID v_pid_x1 = {0.028, 0, 0, 9, 0, 0, 0.02, 10, 20};

PID a_pid_y1 = {7, 0, 0, 0, 0, 0, 0.02, 10, 15};
PID v_pid_y1 = {0.028, 0, 0, 9, 0, 0, 0.02, 10, 20};

float set_ = 0;

extern int set_d_x;
extern int set_d_y;

float pid_col(PID *x)
{
	float out = 0;
	float pv, iv, dv;
	
	pv = (x->p) * (x->error);
	
	x->ierror += x->error;
	iv = (x->i) * (x->ierror);
	if (iv > (x->i_max)) iv = (x->i_max);
	else if (iv < -1 * (x->i_max)) iv = -1 * (x->i_max);
	
	dv = (x->d) * ((x->error) - (x->last_error)) / (x->t0);
	x->last_error = x->error;
	
	out = pv + iv + dv;
	
	if (out > x->out_max) out = x->out_max;
	if (out < -1 * x->out_max) out = -1 * x->out_max;
	return out;
}

//����
void PID_init(void)
{
	v_pid_x.error = 0;
	v_pid_x.ierror = 0;
	v_pid_x.last_error = 0;
	
	a_pid_x.error = 0;
	a_pid_x.ierror = 0;
	a_pid_x.last_error = 0;
	
	v_pid_y.error = 0;
	v_pid_y.ierror = 0;
	v_pid_y.last_error = 0;
	
	a_pid_y.error = 0;
	a_pid_y.ierror = 0;
	a_pid_y.last_error = 0;
	
	TIM_SetCompare2(TIM2, 154);//balck
	TIM_SetCompare3(TIM2, 154);//red
}

//����PID�����㷨
void Contol_Algorithm_x(int now_d)
{
	float v_out;  //�����ٶȵ����
	float a_out;  //�����������
	float now_v = 0;  //ʵ���ٶ�
 //	if(set_d==40) v_pid_x.p=0.025;  //�޷�
	//�ٶȻ�PID��ͨ���ٶȿ���λ��
	v_pid_x.error = set_d_x - now_d;  //���룺λ��ƫ��  
	now_v = (v_pid_x.last_error - v_pid_x.error);  //����ʵ���ٶ�
	v_out = pid_col(&v_pid_x);  //����������ٶȣ������ٶȻ���Ŀ��ֵ
	
	//printf("v_out:%.2f\r\n", v_out);
  //v_out=3;
	//���ٶȻ�PID��ͨ�����ٶȿ����ٶ�
	a_pid_x.error = v_out - now_v;  //���룺�ٶ�ƫ��
	a_out = pid_col(&a_pid_x);  //��������Ƽ��ٶ�
	printf("now_v:%.2f\n", now_v);
	//OLED_ShowNum(2,1,100 * (((float)a_out+90)/(float)90+0.5),4);
	//OLED_ShowNum(3,1,v_out,4);
	CTRL_X(a_out);
}

void Contol_Algorithm_y(int now_d)
{
	float v_out;  //�����ٶȵ����
	float a_out;  //�����������
	float now_v = 0;  //ʵ���ٶ�
 //	if(set_d==40) v_pid_y.p=0.025;  //�޷�
	//�ٶȻ�PID��ͨ���ٶȿ���λ��
	v_pid_y.error = set_d_y - now_d;  //���룺λ��ƫ��  
	now_v = (v_pid_y.last_error - v_pid_y.error);  //����ʵ���ٶ�
	v_out = pid_col(&v_pid_y);  //����������ٶȣ������ٶȻ���Ŀ��ֵ
	
	//printf("v_out:%.2f\r\n", v_out);
  //v_out=3;
	//���ٶȻ�PID��ͨ�����ٶȿ����ٶ�
	a_pid_y.error = v_out - now_v;  //���룺�ٶ�ƫ��
	a_out = pid_col(&a_pid_y);  //��������Ƽ��ٶ�
	printf("now_v:%.2f\n", now_v);
	//OLED_ShowNum(2,1,100 * (((float)a_out+90)/(float)90+0.5),4);
	//OLED_ShowNum(3,1,v_out,4);
	CTRL_Y(a_out);
}

void Contol_Algorithm_x1(int now_d)
{
	float v_out;  //�����ٶȵ����
	float a_out;  //�����������
	float now_v = 0;  //ʵ���ٶ�
 //	if(set_d==40) v_pid_x.p=0.025;  //�޷�
	//�ٶȻ�PID��ͨ���ٶȿ���λ��
	v_pid_x1.error = set_d_x - now_d;  //���룺λ��ƫ��  
	now_v = (v_pid_x1.last_error - v_pid_x1.error);  //����ʵ���ٶ�
	v_out = pid_col(&v_pid_x1);  //����������ٶȣ������ٶȻ���Ŀ��ֵ
	
	//printf("v_out:%.2f\r\n", v_out);
  //v_out=3;
	//���ٶȻ�PID��ͨ�����ٶȿ����ٶ�
	a_pid_x1.error = v_out - now_v;  //���룺�ٶ�ƫ��
	a_out = pid_col(&a_pid_x1);  //��������Ƽ��ٶ�
	printf("now_v:%.2f\n", now_v);
	//OLED_ShowNum(2,1,100 * (((float)a_out+90)/(float)90+0.5),4);
	//OLED_ShowNum(3,1,v_out,4);
	CTRL_X(a_out);
}

void Contol_Algorithm_y1(int now_d)
{
	float v_out;  //�����ٶȵ����
	float a_out;  //�����������
	float now_v = 0;  //ʵ���ٶ�
 //	if(set_d==40) v_pid_y.p=0.025;  //�޷�
	//�ٶȻ�PID��ͨ���ٶȿ���λ��
	v_pid_y1.error = set_d_y - now_d;  //���룺λ��ƫ��  
	now_v = (v_pid_y1.last_error - v_pid_y1.error);  //����ʵ���ٶ�
	v_out = pid_col(&v_pid_y1);  //����������ٶȣ������ٶȻ���Ŀ��ֵ
	
	//printf("v_out:%.2f\r\n", v_out);
  //v_out=3;
	//���ٶȻ�PID��ͨ�����ٶȿ����ٶ�
	a_pid_y1.error = v_out - now_v;  //���룺�ٶ�ƫ��
	a_out = pid_col(&a_pid_y1);  //��������Ƽ��ٶ�
	printf("now_v:%.2f\n", now_v);
	//OLED_ShowNum(2,1,100 * (((float)a_out+90)/(float)90+0.5),4);
	//OLED_ShowNum(3,1,v_out,4);
	CTRL_Y(a_out);
}

void CTRL_X(int out)
{
	TIM_SetCompare2(TIM2, 100 * (((float)out+93)/(float)90+0.5));
}

void CTRL_Y(int out)
{
	TIM_SetCompare3(TIM2, 100 * (((float)out+93)/(float)90+0.5));
}

