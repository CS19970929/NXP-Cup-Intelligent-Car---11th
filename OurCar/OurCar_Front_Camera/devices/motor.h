#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

//��������10000������Ҫ�󲻴���98%
#define PWM_MAX  9700
#define PWM_MIN  0//-9700

void Motor_Init(uint32_t freq);
void Servo_Init(uint32_t freq);
void Motor_PWM(int32_t Left_PWM,int32_t Right_PWM);

#endif
