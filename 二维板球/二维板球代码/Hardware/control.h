#ifndef __control_H
#define __control_H
#include "sys.h"

typedef struct Kpid {
	float ki;
	float kp;
	float kd;
}kpid_t; 

#define X_AngleBias         0
#define Y_AngleBias         0
#define Angle2Compare(x)  (u16)(100 * ((float)x/(float)90+0.5))  //将想设置的舵机角度转换成pwm的比较值
#define CTRL_X(angle)	    TIM_SetCompare3(TIM2, Angle2Compare((angle+X_AngleBias)))

extern void PID_Control_v(int position_x, int aim_x);
extern void PID_Control_a(int position_v, int aim_v);
extern void Control_Reset(void);
int limit_angle_v(int angle,int flag);
int limit_angle_a(int angle,int flag);

#endif 

