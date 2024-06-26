#include "pid.h"
#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "STEPMOTOR.h"
#include "math.h"

PID v_pid_x = {0.00136 * 0.5, 0, 0, 0, 0, 0, 0.02, 0, 0.1};


float pid_col(PID *x)
{
	float out = 0;
	float pv=0, iv=0, dv=0;
	
	pv = (x->p) * (x->error);

//	x->ierror += x->error;
//	iv = (x->i) * (x->ierror);
//	if (iv > (x->i_max)) iv = (x->i_max);
//	else if (iv < -1 * (x->i_max)) iv = -1 * (x->i_max);
//	
//	dv = (x->d) * ((x->error) - (x->last_error)) / (x->t0);
//	x->last_error = x->error;
	
	out = pv + iv + dv;
	
	if (out >  x->out_max) out = x->out_max;
	if (out <  -x->out_max) out = -1 * x->out_max;
	//printf("pv:%f iv:%f  dv:%f out:%f ",pv,iv,dv,out);
	return out;
}

void PID_init(PID *x)
{
	x->error = 0;
	x->ierror = 0;
	x->last_error = 0;
}

void Contol_Algorithm(int now_x, int now_y,int set_x, int set_y)
{
	if(abs(set_x - now_x) <= 2)
	{
		int error = set_y - now_y;
		float p = v_pid_x.p;
		float out = p *(float)error;
		Set_y(-out);
//		printf("error:%d  p:%d   out_y:%f\r\n",error,p,out);
		return;
	}
	
	float v_out;  //期望速度的输出
 	
	v_pid_x.error = set_x - now_x;  //输入：位移偏差  
	v_out = pid_col(&v_pid_x);  //输出：控制速度，即加速度环的目标值
	
	float k = 0;
	k=(set_y-now_y)/v_pid_x.error;
	
	//printf("shang:%d     set_x:%d    now_x:%d   x_out:%f  \r\n",(set_y-now_y),set_x,now_x,v_out);
	if( (-k * v_out) < -0.1)
		Set_y(-0.1);
	else if( (-k * v_out) > 0.1)
		Set_y(0.1);
	else
	{
		Set_y(-k*v_out);
	}
//	printf("k:%f  y_out:%f\r\n",k, -k * v_out);
	Set_y(-k*v_out);
	Set_x(v_out);
//	printf("k: %f -k*v_out:%f  v_out:%f\r\n", k, -k * v_out*1000, -v_out);
	
}
