#include "common.h"
#include "servo.h"
#include "ftm.h"

struct dPID d_PID;

#if C11
float Servo_MID = 1468;//1325;//1390          // �����ֵ
float Servo_MAX =1680  ;//1520;//1580          // ������ֵ
float Servo_MIN = 1259;//1130;//1200          // �����Сֵ
float Servo_Error = 300;
#else
float Servo_MID = 1410;//1390          // �����ֵ
float Servo_MAX = 1640;//1580          // ������ֵ
float Servo_MIN = 1170;//1200          // �����Сֵ
float Servo_Error = 240;
#endif

//���PID����
float Direct_P=4.7;
float Direct_I=0.0;
float Direct_D=10.7;
	
void Servo_Init(uint32_t freq)
{
FTM_PWM_QuickInit(FTM1_CH1_PA09, kPWM_EdgeAligned, freq);
FTM_PWM_ChangeDuty(HW_FTM1,1,Servo_MID);
	

}

void Servo_MID_Set(void)
{
	
}

 
//PID��ʼ��
void dPIDInit( struct dPID *rPID ) {

  rPID->SetPoint = 0 ;   
  rPID->NowPoint = 0 ;   
  
  rPID->Kp = 0 ;  
  rPID->Kd = 0 ;
    
  rPID->LastError = 0 ; //Error[-1]
     
  rPID->PreD = 0 ;      //λ��ʽPID�����������λ��ֵ     
}






  //����ƫ���趨PID
  void Set_ServoParameter(struct dPID *pp_d) {
      pp_d->Kp = Direct_P;
      pp_d->Kd = Direct_D;
  }

                    
	
	
extern float ex_mid;

    //�Ƕȿ���
void AngleCtl( struct dPID *pp, int16_t rMid_Line ) {  
    uint16_t rDirection;
    //�趨����
    Set_ServoParameter( pp );
    //�趨Ŀ��Ϊ����ֵ ������Ϊ0
    pp->SetPoint =ex_mid ;
    //ʵ�ʷ����Ӧ������ֵ
    pp->NowPoint = rMid_Line ; 
    //����PID����
    //�Ӽ��������������ʻȷ��
    rDirection = Servo_MID - dPIDCalc ( pp ) ;
    //����޷�
    if( rDirection > Servo_MAX )  rDirection = Servo_MAX ;
    else if( rDirection < Servo_MIN )  rDirection = Servo_MIN ; 
		
    FTM_PWM_ChangeDuty(HW_FTM1,1,rDirection);
  }
  
	
	
//----------------------------------------------------------------------------------
//�����λ��ʽPID������ܵ�PWM

int16_t dPIDCalc( struct dPID *d_pp ) 
{
  //���嵱ǰƫ�ƫ���΢��
  int16_t dError,Error;
  //���㵱ǰƫ��
  Error = d_pp->SetPoint - d_pp->NowPoint;        //ƫ��
  //ƫ��΢��
  dError = Error - d_pp->LastError;             //΢��
  d_pp->LastError = Error;
  //����λ��PID
  d_pp->PreD = (int32_t) (d_pp->Kp * Error
                    + d_pp->Kd * dError) ;
  //����λ��PID 
  if(d_pp->PreD >  Servo_Error)d_pp->PreD =  Servo_Error;
  if(d_pp->PreD < 0 - Servo_Error)d_pp->PreD = 0 - Servo_Error;
  
  return  (int16_t)d_pp->PreD ;
}




