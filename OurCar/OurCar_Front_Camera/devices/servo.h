#ifndef __SERVO_H__
#define __SERVO_H__


#include <stdint.h>

#define EX_MID 40 

#define C11 1


 //�����λ��ʽPID
  struct dPID            //����������������
  {
    int16_t SetPoint;      //�趨Ŀ�� desired value
    int16_t NowPoint;      //��ǰ������
    
    double Kp;           //��������
    double Kd;           //΢�ֳ���
    
    int16_t LastError;      //Error[-1]
    
    int32_t PreD;           //���λ��
  };
 
	
void dPIDInit( struct dPID *rPID );
void AngleCtl( struct dPID *pp, int16_t rMid_Line );
int16_t dPIDCalc( struct dPID *d_pp ) ;


#endif

