#ifndef __ONCHIP_FLASH_H
#define __ONCHIP_FLASH_H
#include <stdint.h>
#include <stdbool.h>
/* chip's Flash size and sector size can be found in RM */
#define DEV_SIZE            (0x80000)
#define Sector_Base          224
#define Addr_Base            Secter_Base*SECTOR_SIZE     //458752->0x70000

/*
!!!
�����洢������224��ʼ��һ������ֻ�洢һ�����������255���������ִ洢����ֻ�ܴ洢32������ 
!!!
*/

//Ƭ����Դ�㹻�����Բ���һ����������ʹ��һ�������ķ�ʽ��һ������Ϊ2Kbyte->2048byte
//��������洢�������������ַ��ƫ��
#define Dir_Preset_Offset  			 0
#define Speed_EX_H_Offset    		 1
#define Speed_EX_L_Offset 			 2
#define L_ex_mid_Offset   			 3
#define R_ex_mid_Offset   			 4
#define X_Thre_Offset     			 5
#define Ar_max_Offset      			 6
#define Ar_min_Offset     			 7
#define Wr_min_Offset     			 8
#define Beacon_N_Offset  			   9
#define Obstacle_X_Thre_Offset   10
#define Lose_MAX_Offset          11
#define Threshold_Offset         12
#define Cnt_X_Thre_Offset        13
#define Servo_MID_Offset         14
#define Servo_MIN_Offset         15
#define Servo_MAX_Offset         16
#define Obs_preset_Offset        17

void Read_Var(uint8_t offset);
void Write_All_Var(void);
void Write_Var(uint8_t offset);
void Read_All_Var(void);
void FlashData_Init(void);
	

#endif







