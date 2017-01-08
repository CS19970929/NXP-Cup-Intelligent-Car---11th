#include "common.h"
#include "servo.h"
#include "ftm.h"

struct dPID d_PID;



  //���PID����
  double d_Kp   =  9.0;
  double d_Kd   =  1.7;

	
	
	void Servo_Init(uint32_t freq)
{
FTM_PWM_QuickInit(FTM1_CH0_PA08, kPWM_EdgeAligned, freq);
FTM_PWM_ChangeDuty(HW_FTM1,0,Servo_MID);
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
      pp_d->Kp = d_Kp;
      pp_d->Kd = d_Kd;
  }

                    
  //�Ƕȿ���
  void AngleCtl( struct dPID *pp, int16_t rMid_Line ) {  
    uint16_t rDirection;
    //�趨����
    Set_ServoParameter( pp );
    //�趨Ŀ��Ϊ����ֵ ������Ϊ0
    pp->SetPoint =mid ;
    //ʵ�ʷ����Ӧ������ֵ
    pp->NowPoint = rMid_Line ; 
    //����PID����
    //�Ӽ��������������ʻȷ��
    rDirection = Servo_MID + dPIDCalc ( pp ) ;
    //����޷�
    if( rDirection > Servo_MAX )  rDirection = Servo_MAX ;
    else if( rDirection < Servo_MIN )  rDirection = Servo_MIN ; 
		
    FTM_PWM_ChangeDuty(HW_FTM1,0,rDirection);
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




