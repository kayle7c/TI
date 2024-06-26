#include "pid.h"
#include "usart.h"
#include "pwm.h"
#include "OLED.h"
#include "lcd.h"
#include "lcd_init.h"


int OBJ_SEAT[6] = {0, 50, 150, 250, 350, 450};  //��ÿ���±��λ��mm
//PID a_pid = {30, 0, 0, 0, 0, 0, 0.02, 10, 15};
PID v_pid = {20, 0, 0, 0, 0, 0, 0.02, 10, 500};//d 15 p0.028 aim157
//float set_v = 0;
int set_d=0;

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
	v_pid.error = 0;
	v_pid.ierror = 0;
	v_pid.last_error = 0;
	
//	a_pid.error = 0;
//	a_pid.ierror = 0;
//	a_pid.last_error = 0;
	
}

//����PID�����㷨
void Contol_Algorithm(int now_d)
{
	float v_out;  //�����ٶȵ����
	float a_out;  //�����������
	float now_v = 0;  //ʵ���ٶ�
 	
	//�ٶȻ�PID��ͨ���ٶȿ���λ��
	v_pid.error = now_d;  //���룺λ��ƫ��  
	now_v = (v_pid.last_error - v_pid.error);  //����ʵ���ٶ�
	v_out = pid_col(&v_pid);  //����������ٶȣ������ٶȻ���Ŀ��ֵ
	
	printf("out:%.2f\r\n", v_out);
  //v_out=3;
	//���ٶȻ�PID��ͨ�����ٶȿ����ٶ�
//	a_pid.error = v_out - now_v;  //���룺�ٶ�ƫ��
//	a_out = pid_col(&a_pid);  //��������Ƽ��ٶ�
//	printf("now_v:%.2f\n", now_v);
	//OLED_ShowNum(2,1,100 * (((float)a_out+90)/(float)90+0.5),4);
	//OLED_ShowNum(3,1,v_out,4);
	//OLED_ShowNum(4,1,a_out,4);
	CTRL_X(v_out);
}

void CTRL_X(int out)
{
//	printf("out:%d\r\n",out+2000);
	if(out<0) 	TIM_SetCompare3(TIM2,1000+out*1.8);
	else 	TIM_SetCompare3(TIM2,1000+out);
}

