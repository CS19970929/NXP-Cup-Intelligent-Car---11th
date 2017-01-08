/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ���ѧ��̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       VCAN_SCCB.h
 * @brief      OV����ͷ��������SCCB������
 * @author     ɽ��Ƽ�
 * @version    v5.0
 * @date       2013-09-01
 */


#ifndef __SCCB_H__
#define __SCCB_H__

#include <stdint.h>
#include "gpio.h"


//SCCB �ܽ�����PTA25 26
#define SCCB_SCL        26
#define SCCB_SDA        25


#define SCCB_SCL_H()         PAout(SCCB_SCL) = 1
#define SCCB_SCL_L()         PAout(SCCB_SCL) = 0
#define SCCB_SCL_DDR_OUT()   PTA->PDDR|=(1<<SCCB_SCL) 
#define SCCB_SCL_DDR_IN()    PTA->PDDR&=~(1<<SCCB_SCL) 

#define SCCB_SDA_H()         PAout(SCCB_SDA) = 1
#define SCCB_SDA_L()         PAout(SCCB_SDA) = 0
#define SCCB_SDA_IN()        PAin(SCCB_SDA)
#define SCCB_SDA_DDR_OUT()   PTA->PDDR|=(1<<SCCB_SDA)
#define SCCB_SDA_DDR_IN()    PTA->PDDR&=~(1<<SCCB_SDA)

#define ADDR_OV7725   0x42

#define DEV_ADR  ADDR_OV7725             /*�豸��ַ����*/

#define SCCB_DELAY()    SCCB_delay(400)


void SCCB_GPIO_init(void);
int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte);
int SCCB_ReadByte(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress);


#endif      //_VCAN_SCCB_H_
