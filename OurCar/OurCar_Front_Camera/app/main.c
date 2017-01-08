#include <stdio.h>
#include <math.h>
/*Drivers*/
#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "ftm.h"//����PWMͨ��
#include "i2c.h"
#include "adc.h"
#include "pit.h"
#include "sd.h"//���ļĴ���
#include "can.h"
#include "deviceio.h"//���صײ㺯����
#include "flash.h"
#include "uart.h"
/*Devices*/
#include "motor.h"
#include "oled.h"
#include "led.h"
#include "sccb.h"
#include "Eagle.h"
#include "servo.h"
#include "BatteryMon.h"
#include "encoder.h"
#include "anglesensor.h"
#include "key.h"
#include "bluetooth.h"
#include "IR.h"
#include "SR04.h"
#include "adns9800.h"
/*app*/
#include "umc.h"
#include "ctrl.h"
#include "Debug.h"
#include "filter.h"
#include "Get_Beacon.h"
#include "can_communication.h"
#include "onchipflash.h"
#include "findroute.h"

#define Flow_RATIO 331.2    //������ֵ�����ת������
static void PIT0_CallBack(void);
static void PIT1_CallBack(void);

/*****extern ��****************/
extern struct dPID d_PID;//���PD
extern float CarSpeed;
extern uint8 mat[60][80];//�����ͨ��֮���ͼ��
extern float Speed_EX_H;
extern float Speed_EX_L;
extern uint8   *ov7725_eagle_img_buff;
extern IMG_STATUS_e eagle_img_flag;   //ͼ��״̬
extern uint8_t Beacon_cnt;//��¼�ڼ����ű�
extern float Servo_MID;
extern float Servo_MAX;
extern float Servo_MIN;
extern float Beacon_N;//�������ű�
//Debug ר�ñ���
float Debug=0;
float Debug_time=99.7;
uint8 imgbuff[80*60/8];  //��ֵ��ͼ��1 byte--->8 pixel
uint8 pic[60][80];			 //��ѹ���ֵ��ͼ��1 byte--->1 pixel 
uint8_t Frame_Capture_Pic[900];//����ͼ��
struct system System;//ϵͳ��ر���
struct Beacon DstBeacon;//Ŀ���ű�λ��
int16_t Mid_Line = EX_MID;
uint8_t ShowPos_flag=1;			//OLED��ʾλ����ز�����־��0����ʾ��1��ʾ
uint8_t Out_Put_Data_flag=0;//���ڷ������ݱ�־��0�����ͣ�1����
uint8_t Obst_Dir_flag = 0;	//�ϰ���־����¼��������֮��ת��0��ת���ϣ�1��ת����
float Obstacle_Flag = -1;
uint8_t oled_img_flag=0;		//OLED��ʾͼ���־��0����ʾ��1��ʾ
uint8_t uart_img_flag=0;		//���ڴ�ͼ���־��0�����ͣ�1����
uint8_t start_flag=0;				//������ʼ��־��0��ʾ�ȴ�������ʼ��1��ʾ���������У�2��ʾ��������
float Stop_flag=1;					//ͣ����־��0��ͣ����1ͣ��
uint8_t TIME0flag_20ms=0;
uint8_t TIME1flag_5ms=0;
uint8_t TIME1flag_10ms=0;
uint8_t TIME1flag_20ms=0;
uint8_t TIME1flag_100ms=0;
uint8_t TIME1flag_1000ms=0;
float L_ex_mid = 58;
float M_ex_mid = 40;
float R_ex_mid = 22;
float ex_mid = 40;
float Servo_PWM=1300;
uint8_t B_Mid_line_flag=0;//������ͷ��⵽�ű��־
uint8_t B_Mid_line=0;//������ͷ��⵽�ű�λ��
uint8_t F_Mid_line_flag=0;//ǰ����ͷ��⵽�ű��־
int Lose_cnt=0;//���߼���
float Lose_MAX=3.0;
float Obstacle_MAXtime = 8.0;//������ת�����ʱ��
float Obstacle_Straight_time = 0.0;//�����ʱ��
uint8_t Obstacle_time=1;
float Obstacle_X_Thre = 29.0;
uint8_t Lose_flag=0;//���߱�־
uint8_t Turn_flag=0;
float X_Thre = 26;
//float SR04_Distance1=100.0;
//float SR04_Distance2=100.0;
int16_t Motion_X=0,Motion_Y=0;//360--1mm
float X=0.0,Y=0.0;//��λmm
float ADNS_ANGLE=0,ADNS_ANGLE1;//����ͨ��adns��Ƕ�
//��¼ת��ʱ�̵�˲ʱ�ٶ���X��ֵ�����ڵ���
float X_temp=0;
float S_temp=0;
double  voltage;//��Դ��ѹ

int main()
{
	DisableInterrupts;
	/*================��ʱ��ʼ��=================*/
	DelayInit();
	/*=================��λ��ͨ��=================*/
  UMC_Init();
	DelayMs(10);
	UART_printf("\nUMC_Init Success!\n");
	/*================LED��ʼ��=================*/
	UART_printf("LED_Init...\n");
  LED_Init();
	/*=================OLED��ʼ��=================*/
	UART_printf("OLED_Init...\n");
	OLED_Init();
	/*=================��ص�ѹ��ʼ��=================*/
	UART_printf("BatteryMon_Init...\n");
	BatteryMon_Init();
	/*=================�����ʼ��,Ƶ��10k=================*/
	UART_printf("Motor_Init...\n");
	Motor_Init(10000);
	/*=================�����ʼ��=================*/
	UART_printf("Servo_Init...\n");
	Servo_Init(100);
	/*=================CANͨ�ų�ʼ��=================*/
	UART_printf("Can_Init...\n");
	Can_Init();
	/*=================ӥ�۳�ʼ��=================*/
	UART_printf("Eagle_Init...\n");
  Eagle_Init(imgbuff);
	/*=================PIT0��ʼ��,2ms��ʱ�ж�=================*/
	UART_printf("PIT_QuickInit...\n");
	PIT_QuickInit(HW_PIT_CH0,2000);
  PIT_CallbackInstall(HW_PIT_CH0,PIT0_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH0,kPIT_IT_TOF,ENABLE);
	/*=================PIT1��ʼ��,5ms��ʱ�ж�=================*/	
	PIT_QuickInit(HW_PIT_CH1,5000);
	PIT_CallbackInstall(HW_PIT_CH1,PIT1_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH1,kPIT_IT_TOF,ENABLE);
	/*=================������ճ�ʼ��=================*/	
//	UART_printf("IR_Init...\n");	
//  IR_Init();
	/*=================��������ʼ��=================*/
//	UART_printf("SR04_Init...\n");	
//	SR04_Init();
	/*=================������ʼ��=================*/
//	UART_printf("ADNS9800_init...\n");
//	ADNS9800_init();
	/*====================���̳�ʼ��=======================*/
	UART_printf("Key_Init...\n");
	Key_Init();
	/*=================�����ֳ�ʼ��=================*/
	UART_printf("M_Encoder_Init...\n");
	M_Encoder_Init();
	/*================Ƭ��flash��ʼ��=============*/
	UART_printf("FLASH_Init...\n");
	FLASH_Init();
	/*================Ƭ��flash���ݳ�ʼ��=============*/
	UART_printf("FlashData_Init...\n");
	FlashData_Init();
	/*=================���������ʼ��=================*/
	UART_printf("ParameterList_Init...\n");
	ParameterList_Init();
	/*================׼��������ʼ=================*/
	UART_printf("Car_Start...\n");
	Car_Start();
	/*================�����ж�=================*/
	UART_printf("EnableInterrupts!\n");
	EnableInterrupts;
	UART_printf("MajorLoop Start!\n");
//���������ٶ�����ʹ��
//while(1)
//{
//if(TIME1flag_20ms==1)
//{
//	DelayMs(5);
//	//Can_Send(Speed_ID,Speed_EX_H);
//	FTM_PWM_ChangeDuty(HW_FTM1,1,Servo_PWM);
//	Motor_PWM(0,5000);
//	//;Can_Send(DiffSpeed_ID,0);
//	Key_Scan();
//	Parameter_Change();
//} 
//}
	while(1)
	{
		/*================================================================*/
		if(TIME0flag_20ms==1)
		{
			TIME0flag_20ms=0;
		}
		if(TIME1flag_5ms==1)
		{
			TIME1flag_5ms=0;
		}
		if(TIME1flag_10ms==1)
		{
			TIME1flag_10ms=0;
//����ƻ�ʹ�ù�����λ��֮�����			
//			Get_Motion(&Motion_X,&Motion_Y);
//			X+=(float)Motion_X/Flow_RATIO;
//			Y+=(float)Motion_Y/Flow_RATIO;
//		  sprintf(str,"%-9.0f",Y);
//	    OLED_Print(0,6,(uint8_t*)str);
//			ADNS_ANGLE= atan2(Motion_Y,Motion_X) * (180 / 3.14159265);
//			ADNS_ANGLE1= atan2(Y,X) * (180 / 3.14159265);
			Key_Scan();
		}
		if(TIME1flag_20ms==1)
		{
			TIME1flag_20ms=0;
			//��ʼ�ɼ���һ֡ͼ��
		  eagle_get_img();
			//��ѹͼ��600=80*60/8
			img_extract(&pic[0][0],imgbuff,600);
			//ɨ��ʶ���ű�
			GetBeacon(mat,&DstBeacon);
			//LEDָʾ״̬
			LED_Switch(F_Mid_line_flag);
			//Debug  ���ڶ�ʱͣ���ȣ�����ʶ��֮�󣬿���֮ǰ
			Debug_Check();
			//���Ʋ���
			if(F_Mid_line_flag==1&&start_flag!=2)//ǰ����ͷ�ҵ��ű��ұ���û�н���
			{
				Lose_flag=0;
				Lose_cnt=0;
				start_flag=1;	
				Mid_Line  = DstBeacon.y;
				Beacon_CNT();
				if(DstBeacon.x<=Obstacle_X_Thre&&Obstacle_time>1)
				{
					if(Obstacle_time<=Obstacle_Straight_time)FTM_PWM_ChangeDuty(HW_FTM1,1,Servo_MID);
					if(Obstacle_time>1)Obstacle_time--;
				}else if(DstBeacon.x>X_Thre)
				{
					if(Turn_flag>3)
					{
						X_temp=X_Thre;
						S_temp=CarSpeed;
						//sprintf(str,"%d,%3.2f\n",(int)X_temp,S_temp);
						//UART_putstr(HW_UART0,str);
					}
					Turn_flag=1;
					Path_Finding();
				}
				else{
					Turn_flag++;
					if(Turn_flag>3)Turn_flag=4;
					Path_Runing();
					Check_Obstacle();
				}	
			}else if(start_flag==1)//û��ʶ���ű��ұ���������
			{ 
				Lose_cnt++;
				if(DstBeacon.x<=Obstacle_X_Thre&&Obstacle_time>1)
				{
					if(Obstacle_time<=Obstacle_Straight_time)FTM_PWM_ChangeDuty(HW_FTM1,1,Servo_MID);
					if(Obstacle_time>1)Obstacle_time--;
				}else if(Lose_cnt>=Lose_MAX)
				{	
					Lose_cnt=Lose_MAX+1;
					Path_Circling();
					if(Stop_flag>=1&&Beacon_cnt>=Beacon_N-1&&Turn_flag==1)
					{
						Car_Stop();
					}
				}
			}
			//�������ݸ���λ��	
			if(Out_Put_Data_flag)
			{
				Out_Data[0]=(int)((X_Thre-23)*100);
				Out_Data[1]=(int)(CarSpeed*100);
				Out_Data[2]=(int)(0);
				Out_Data[3]=(int)(Beacon_cnt*50);
				Out_Put_Data();		
			}
			if(oled_img_flag)//����4���¸ı�oled_img_flag��Ϊ1ʱͨ��Oled��ʾ��ֵ��ͼ��
			{
			   Oled_Img(&pic[0][0]);
			}
			if(uart_img_flag)//����3���¸ı�uart_img_flag��Ϊ1ʱͨ������0���Ͷ�ֵ��ͼ��
			{
				//sendimg(&mat[0][0],80*60);//�����ͨ����ͼ��
				sendimg(imgbuff,600);//��ֵ��ͼ��600=80*60/8
			}
			Show_Pos(ShowPos_flag);		
		}
		if(TIME1flag_100ms==1)
		{
			TIME1flag_100ms=0;	
		}
		if(TIME1flag_1000ms==1)
		{
			TIME1flag_1000ms=0;
      //�ɼ���ص�ѹ
			voltage=Get_BatVol();
		}
		//����������¼�����
		Parameter_Change();
	}
}

static void PIT0_CallBack()
{
	static uint8_t pit0_cnt=0;	
	pit0_cnt++;
	//������ɨ��
  M_Encoder_Scan();
	System.time+=0.002;
	if(pit0_cnt>=10)
	{
		pit0_cnt=0;
		TIME0flag_20ms=1;
	}	
}

static void PIT1_CallBack()
{
	static uint8_t pit1_cnt=0;
	
	pit1_cnt++;
	if(pit1_cnt%1==0)
	{
		TIME1flag_5ms=1;
	}
	if(pit1_cnt%2==0)
	{
		TIME1flag_10ms=1;
	}
	if(pit1_cnt%4==0)
	{
		TIME1flag_20ms=1;
	}
	if(pit1_cnt%20==0)
	{
		TIME1flag_100ms=1;
	}
	if(pit1_cnt%200==0)
	{
		TIME1flag_1000ms=1;
	}
	if(pit1_cnt>=200)
	{
		pit1_cnt=0;
	}
}


