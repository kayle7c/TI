#ifndef __PID_H
#define __PID_H	 
#include "sys.h"

typedef struct pid
{
	float p, i, d;
	float error;
	float last_error;
	float ierror;
	float t0;
	float i_max;
	float out_max;
}PID;

extern float PID_MID;
extern PID a_pid;
extern PID v_pid;

float pid_col(PID *x);
void PID_init(PID *x);
void Contol_Algorithm(int now_x, int now_y,int set_x, int set_y);

		 				    
#endif
