#ifndef __PID_H
#define __PID_H	 
#include "sys.h"

typedef struct node
{
	float p, i, d;
	float error;
	float last_error;
	float ierror;
	float t0;
	float i_max;
	float out_max;
}PID;

//#define Angle2Compare(x)  (u16)(100 * ((float)x/(float)90+0.5))  //将想设置的舵机角度转换成pwm的比较值
//#define CTRL_X(angle)	    TIM_SetCompare3(TIM2, Angle2Compare((angle)+90))

extern float PID_MID;
//extern PID a_pid;
extern PID v_pid;

float pid_col(PID *x);
void PID_init(void);
void Contol_Algorithm(int now_d);
void CTRL_X(int out);
		 				    
#endif
