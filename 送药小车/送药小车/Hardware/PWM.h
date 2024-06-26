#ifndef PWM_H
#define PWM_H
#include "sys.h"

void MOTOR_PWM_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void PWM_SetCompare4(uint16_t Compare);

#endif
