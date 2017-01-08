#ifndef __CAN_COMMUNICATION_H__
#define __CAN_COMMUNICATION_H__

#include "stdlib.h"
#include "stdint.h"
#define CAN_TX_MB   12
#define CAN_TX_ID   0x01      //����ID
#define CAN_RX_ID   0x02      //����ID

#define Speed_ID     0x01//���ͽ��տ�����Ϣ
#define Obstacle_ID  0x02//�����ϰ���Ϣ
#define X_Thre_ID    0x03//���ռ����X_Thre
#define R_Speed_ID   0x04//���ղɼ���ʵʱ�ٶ�
#define distance_ID  0x05//���ջ��ֵ���̳���
#define System_ID    0x06//ϵͳ��ز��� data<0��������
#define DiffSpeed_ID 0x07//�������ӵ����ջ���������

typedef struct 
{
	  uint8_t data[8];
    uint8_t len;
    uint32_t id;
}CAN_Message_Type;




void Can_Init(void);
void Can_Send(uint8_t ID,float data);
void Choose_X_Thre(float speed);
void Can_SendDiffSpeed(uint8_t ID,float data,float data1);
#endif



