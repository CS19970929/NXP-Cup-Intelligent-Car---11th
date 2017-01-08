#ifndef __CTRL_H
#define __CTRL_H

#include <stdint.h>

#define Ts       0.02
//����תһȦ����563���������ܳ�0.163������0.02


#define C11 1

#if C11
#define SPEED_RATIO ((0.163/563)/Ts)
#else
#define SPEED_RATIO ((0.163/593)/Ts)
#endif


void SetSpeedPI(int p,int i);
void SetMotorDead(int val);


void SpeedCalculate(void);
void SpeedCtrl(void);
void DiffSpeedCtrl(void);
void DirectCtrl(void);
void Choose_X_Thre(float speed);

#endif
