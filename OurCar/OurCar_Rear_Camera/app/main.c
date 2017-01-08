#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
#include "usmart.h"
#include "znfat.h"
#include "deviceio.h"//���صײ㺯����
/*Devices*/
#include "motor.h"
#include "oled.h"
#include "led.h"
#include "sccb.h"
#include "Eagle.h"
#include "servo.h"
#include "ST7735R.h"
//#include "battery.h"
#include "BatteryMon.h"
#include "encoder.h"
#include "anglesensor.h"
#include "key.h"
#include "bluetooth.h"
#include "IR.h"
#include "ccd.h"
/*app*/
#include "umc.h"
#include "ctrl.h"
#include "Debug.h"
#include "filter.h"
#include "Get_Bacon.h"
#include "can_communication.h"


extern uint8_t BlackM[60];
extern struct dPID d_PID;
uint8_t Mid_Line;
float Q_Z=30;

uint8_t oled_img_flag=0;
uint8_t oled_ccd_flag=0;
uint8_t uart_img_flag=0;
uint8_t TIME0flag_20ms=0;
uint8_t TIME0flag_40ms=0;
uint8_t TIME1flag_5ms=0;
uint8_t TIME1flag_10ms=0;
uint8_t TIME1flag_20ms=0;
uint8_t TIME1flag_100ms=0;
uint8_t TIME1flag_1000ms=0;
static void PIT0_CallBack(void);
static void PIT1_CallBack(void);
extern float Speed_EX;
extern float Acc_Z,Gyro_AR2;
extern float Gyro_Integ;
extern float Angle;
extern float Acc_Sigma;
extern float CarSpeed;
extern uint8 mat[60][80];
extern uint8_t B_Mid_line_flag;//������ͷ�ű���ձ�־
uint8_t LED_FLAG=0;
float CCD_ShowBase = 60/3;

uint8 imgbuff[80*60/8];    
uint8 pic[60][80]; 


struct Beacon DstBeacon;//Ŀ���ű�λ��
struct system System;//ϵͳ��ر���
char str[27];

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
	/*=================��������ʼ��=================*/
	UART_printf("Encoder_Init...\n");
	Encoder_Init();
	/*=================CANͨ�ų�ʼ��=================*/
	UART_printf("Can_Init...\n");
	Can_Init();
	/*=================����CCD��ʼ��=================*/
//	UART_printf("CCD_Init...\n");
//	CCD_Init();
	/*=================ӥ�۳�ʼ��=================*/
//	UART_printf("Eagle_Init...\n");
//  Eagle_Init(imgbuff);
	/*=================PIT0��ʼ��,0.2ms��ʱ�ж�=================*/
	UART_printf("PIT_QuickInit...\n");
	PIT_QuickInit(HW_PIT_CH0,2000);
  PIT_CallbackInstall(HW_PIT_CH0,PIT0_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH0,kPIT_IT_TOF,ENABLE);
	/*=================PIT1��ʼ��,5ms��ʱ�ж�=================*/	
	PIT_QuickInit(HW_PIT_CH1,5000);
	PIT_CallbackInstall(HW_PIT_CH1,PIT1_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH1,kPIT_IT_TOF,ENABLE);
	/*====================���̳�ʼ��=======================*/
	UART_printf("Key_Init...\n");
	Key_Init();
	UART_printf("M_Encoder_Init...\n");
	M_Encoder_Init();
	UART_printf("ParameterList_Init...\n");
	ParameterList_Init();
	Data_Init();
	UART_printf("EnableInterrupts!\n");
	EnableInterrupts;
	UART_printf("MajorLoop Start!\n");
	

	while(1)
	{
		/*================================================================*/
		
		if(TIME0flag_20ms==1)
		{
			TIME0flag_20ms=0;
			
			if(oled_ccd_flag==1)
			{
				Oled_Ccd((uint8_t)CCD_ShowBase,CCD_Data);
			}else 
			{	
//				extern Encoder_Speed_Type LeftSpeed,RightSpeed;		
//				sprintf(str,"%-5d",LeftSpeed.value);
//				OLED_Print(0,6,(uint8_t*)str);
//				sprintf(str,"%-5d",RightSpeed.value);
//				OLED_Print(40,6,(uint8_t*)str);
				sprintf(str,"%-5.2f",System.distance);
				OLED_Print(0,6,(uint8_t*)str);
				
				sprintf(str,"%-5.3f",Speed_EX);
				OLED_Print(80,4,(uint8_t*)str);
				sprintf(str,"%-5.3f",CarSpeed);
				OLED_Print(80,6,(uint8_t*)str);
			}
			if(uart_img_flag==1)sendccd(CCD_Data);
			
//			Out_Data[0]=(int)(Speed_EX*100);
//			Out_Data[1]=(int)(CarSpeed*100);
//			Out_Data[2]=(int)(0);
//			Out_Data[3]=(int)(0);
//			Out_Put_Data();	
			
		}
		
		if(TIME1flag_5ms==1)
		{
			TIME1flag_5ms=0;
		}
		if(TIME1flag_10ms==1)
		{
			TIME1flag_10ms=0;		
		
		  Key_Scan();	
			

		}
		if(TIME1flag_20ms==1)
		{

			TIME1flag_20ms=0;
			//��ʼ�ɼ���һ֡ͼ��
//		  eagle_get_img();
//			
//			//��ѹͼ��
//			img_extract(&pic[0][0],imgbuff,80*60/8);
//			//ɨ���ű�
//			GetBeacon(mat,&DstBeacon);
//			
//			if(B_Mid_line_flag==1)
//			{
				//LED�л�
				LED_Single(LED_FLAG%LED_CNT);
				LED_FLAG++;
//			}

			
			
			
			
			if(oled_img_flag)//����4���¸ı�oled_img_flag��Ϊ1ʱͨ��Oled��ʾ��ֵ��ͼ��
			{
			   Oled_Img(&pic[0][0]);
			}
			if(uart_img_flag)//����3���¸ı�uart_img_flag��Ϊ1ʱͨ������0���Ͷ�ֵ��ͼ��
			{
				sendimg(&mat[0][0],80*60);
			}

				//��ӡ����
				sprintf(str,"%-3d",DstBeacon.y);
				OLED_Print(81,0,(uint8_t*)str);
				sprintf(str,"%-3d",DstBeacon.x);
				OLED_Print(81,2,(uint8_t*)str);

			
			
			
		}
		if(TIME1flag_100ms==1)
		{
			TIME1flag_100ms=0;		

			
		}
		if(TIME1flag_1000ms==1)
		{
			TIME1flag_1000ms=0;
		}
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
	if(pit0_cnt%10==0)
	{
		SpeedCalculate();
		Can_Send(R_Speed_ID,CarSpeed);
		Can_Send(distance_ID,System.distance);
//		extern float DiffSpeed_flag;
//		if(DiffSpeed_flag==0)
//		SpeedCtrl();
//		else 
		DiffSpeedCtrl();
		TIME0flag_20ms=1;
	}
	if(pit0_cnt>=20)
	{
		pit0_cnt=0;
		TIME0flag_40ms=1;
		//CCD_ImageCapture(CCD_Data);
		//Check_Obstacle(CCD_Data);
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
