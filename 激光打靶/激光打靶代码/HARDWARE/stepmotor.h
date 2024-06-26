#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
#include "sys.h"    

#define STEPMOTOR_STEP_ANGLE 1.8
#define STEPMOTOR_SUBDIVISION 256


void PWM_Init(void);  //初始化
void PWMx_Init(void);
void HGPIO_Init(void);
void HGPIOx_Init(void);
void STEPMOTOR_Set_NY(float n);  //设置转速
void STEPMOTOR_Set_NX(float n);
void STEPMOTOR_Dir_ColY(int mod);  //设置方向
void STEPMOTOR_Dir_ColX(int mod);
void Set_x(float n);
void Set_y(float n);

#endif	


