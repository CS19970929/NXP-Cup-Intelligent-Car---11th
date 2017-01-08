#include "common.h"
#include "UMC.h"
#include "uart.h"
#include "usmart.h"
#include <string.h>




Queue UART_RX_Queue;	//���ڽ��ն���
Queue UART_TX_Queue;	//���ڷ��Ͷ���

uint8_t UART_RX_DATA[UART_REC_LEN]; //���ڽ��յ�����
uint16_t UART_RX_STA[2];	//��������״̬����һ����¼���ݳ��ȣ��ڶ�����¼���ݹ���

//��λ״̬������
//void Reset_Receiver(ReceiverSTA_Type *pReceiver_STA);



//��λ�����ݽ���״̬��
static void UM_DATA_Receiver(ReceiverSTA_Type *pReceiver_STA, uint8_t data)
{
	static Recstate_Type RECSTA=RECOP1;
	if(*pReceiver_STA==ResetReceiver)
	{
		RECSTA=RECOP1;
	}
		
	//�����������
	static uint8_t checksum=0;
	static uint16_t datalen=0;
	
	switch(RECSTA)
	{
		case RECOP1:
		{
			if(data==OP1_BYTE)
			{
				RECSTA=RECOP2;
				checksum=0;
			}
			break;
		}
		case RECOP2:
		{
			if(data==OP2_BYTE)
			{
				RECSTA=RECOP3;
			}
			else if(data==OP1_BYTE) RECSTA=RECOP2;
			else RECSTA=RECOP1;
			break;
		}
		case RECOP3:
		{
			if(data==OP3_BYTE)
			{
				RECSTA=RECLEN1;
				*pReceiver_STA=UM_Receiver;
				Reset_Receiver(pReceiver_STA);	//��λ����״̬��
			}
			else if(data==OP1_BYTE) RECSTA=RECOP2;
			else RECSTA=RECOP1;
			break;
		}
		case RECLEN1:
		{
			checksum+=data;
			datalen=(uint16_t)data<<8;
			RECSTA=RECLEN2;
			break;
		}
		case RECLEN2:
		{
			checksum+=data;
			datalen|=data;
			RECSTA=RECSEL;
			UART_RX_STA[0]=0;
			if(datalen>UART_REC_LEN)	//���ݽ��մ���
			{
				RECSTA=RECOP1;
				*pReceiver_STA=USMART_Receiver;
				UART_RX_STA[0]=0;
			}
			break;
		}
		case RECSEL:
		{
			UART_RX_DATA[UART_RX_STA[0]++]=data;
			checksum+=data;
			if(UART_RX_STA[0]>=datalen)		//����������
			{
				RECSTA=RECCHECK;
			}
			break;
		}
		case RECCHECK:
		{
			if(checksum==data)
			{
				RECSTA=RECEND1;
			}
			else
			{
				//����У�����
				if(data==OP1_BYTE) RECSTA=RECOP2;
				else RECSTA=RECOP1;
				*pReceiver_STA=USMART_Receiver;
				UART_RX_STA[0]=0;
			}
			break;
		}
		case RECEND1:
		{
			if(data==END1_BYTE) RECSTA=RECEND2;
			else
			{
				//���ݽ��մ���
				if(data==OP1_BYTE) RECSTA=RECOP2;
				else RECSTA=RECOP1;
				*pReceiver_STA=USMART_Receiver;
				UART_RX_STA[0]=0;
			}
			break;
		}
		case RECEND2:
		{
			if(data==END2_BYTE)
			{
				//���ݽ��ճɹ�
				UART_RX_STA[1]=UM_UART_DATA;	//������ݹ���
				RECSTA=RECOP1;
				*pReceiver_STA=USMART_Receiver;
			}
			else
			{
				//���ݽ��մ���
				if(data==OP1_BYTE) RECSTA=RECOP2;
				else RECSTA=RECOP1;
				*pReceiver_STA=USMART_Receiver;
				UART_RX_STA[0]=0;
			}
		}
	}
}

//USMART���ݽ���״̬��
static void USMART_DATA_Receiver(ReceiverSTA_Type *pReceiver_STA, uint8_t data)
{
	static Recstate_Type RECSTA=RECSEL;
	if(*pReceiver_STA==ResetReceiver)
	{
		RECSTA=RECSEL;
	}
	
	switch(RECSTA)
	{
		case RECSEL:
		{
			if(data==0x0DU) RECSTA=RECEND1;
			else 
			{
				if(UART_RX_STA[0]>=UART_REC_LEN-1) //��һ������
				{
					//��������,���մ���
					UART_RX_STA[0]=0;
				}
				UART_RX_DATA[UART_RX_STA[0]++]=data;
			}
			break;
		}
		case RECEND1:
		{
			if(data==0x0AU)
			{
				//�������
				UART_RX_STA[1]=USMART_UART_DATA;	//������ݹ���
				RECSTA=RECSEL;
			}
			else
			{
				//���մ���
				UART_RX_STA[0]=0;
				UART_RX_DATA[UART_RX_STA[0]++]=data;
				RECSTA=RECSEL;
			}
		}
		default :
			;
	}
}

//��λ״̬������
static void Reset_Receiver(ReceiverSTA_Type *pReceiver_STA)
{
	ReceiverSTA_Type RESET=ResetReceiver;
	if(*pReceiver_STA!=UM_Receiver)
	{
		UM_DATA_Receiver(&RESET,0x00U);
	}
	if(*pReceiver_STA!=USMART_Receiver)
	{
		USMART_DATA_Receiver(&RESET,0x00U);
	}
}

//���ն��д�����
void UART_DATA_Processor(void)
{
	while((Queue_State(&UART_RX_Queue)!=Queue_Empty) &&
		  (UART_RX_STA[1]==NONE_UART_DATA))
	{
		static ReceiverSTA_Type Receiver_STA;
		uint8_t data;
		
		data=Queue_Get(&UART_RX_Queue);
		switch(Receiver_STA)
		{
			case USMART_Receiver:
				UM_DATA_Receiver(&Receiver_STA,data);
				USMART_DATA_Receiver(&Receiver_STA,data);
				break;
			case UM_Receiver:
				UM_DATA_Receiver(&Receiver_STA,data);
				break;
			default :
				;
		}
	}
}




/******************************************************
*���ڳ�ʼ��
******************************************************/
void UMC_Init(void)
{
	//�˴������ڷ������ݵ���λ��
	Queue_Init(&UART_RX_Queue);
	UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);
	UART_CallbackRxInstall(HW_UART4, UART_RX_ISR);
	Queue_Init(&UART_TX_Queue);
	UART_CallbackTxInstall(HW_UART4, UART_TX_ISR);
	//��Ҫ��������ʱ����
	//UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, true);
	//usmart_init(0);
	
	//�˴������ڷ��ͺ�����ͷ���������
	//UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);


	
}




//�����Ǵ��ڴ�����
/***************************************���Ƿָ���***************************************/

static void Queue_Init(Queue * pQueue)
{
	memset(pQueue, 0, sizeof(Queue));
}

static uint8_t Queue_Get(Queue * pQueue)
{
	uint8_t data = pQueue->arr[pQueue->head];
	pQueue->head = (pQueue->head + 1) % QUEUE_SIZE;
	return data;
}

static void Queue_Put(Queue * pQueue, uint8_t ch)
{
	pQueue->arr[pQueue->tail] = ch;
	pQueue->tail = (pQueue->tail + 1) % QUEUE_SIZE;
}

static Queue_State_Type Queue_State(Queue * pQueue)
{
	if (pQueue->head == pQueue->tail)
		return Queue_Empty;
	else if ((pQueue->tail + 1) % QUEUE_SIZE == pQueue->head)
		return Queue_Full;
	else
		return Queue_NotFull;
}

//���ڽ����ж�
static void UART_RX_ISR(uint16_t byteRec)
{
    if(Queue_State(&UART_RX_Queue)==Queue_Full)
	{
		//�����������
	}
	else
	{
		Queue_Put(&UART_RX_Queue,(uint8_t)byteRec);
	}
}

//���ڷ����ж�
//���ڷ����ж�
static void UART_TX_ISR(uint16_t * byteToSend)
{
	
	*byteToSend=(uint16_t)Queue_Get(&UART_TX_Queue);
	if(Queue_State(&UART_TX_Queue)==Queue_Empty)
	{
		UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, false);
	}
}





//�������ݷ��ͺ���
void UART_Send_DATA(uint32_t instance,uint8_t *data, uint16_t len)
{
	uint16_t i;
	
//	for(i=0;i<len;i++)
//	{
//		Queue_Put(&UART_TX_Queue,data[i]);
//		if(Queue_State(&UART_TX_Queue)==Queue_Full)
//		{
//			break;
//		}
//	}
//	
	if(0)
	{
		//������DMA��������
	}
	else if(0)
	{
		//ʹ���жϷ�������
		UART_ITDMAConfig(instance, kUART_IT_Tx, true);
	}
	else
	{
//		while(Queue_State(&UART_TX_Queue)!=Queue_Empty)
//		{
//			UART_WriteByte(instance,(uint16_t)Queue_Get(&UART_TX_Queue));
//		}
//	

		
	for(i=0;i<len;i++)
	{
	  UART_WriteByte(instance,(uint16_t)data[i]);
		
	}
	
	
	
	}
}


/*!
 *  @brief      ����ͼ��eSmartCameraCar��λ����ʾ
 *  @param      imgaddr         ͼ���ַ
 *  @param      imgsize         ͼ��ռ�ÿռ��С
 *  @since      v5.0
 *  @note       ��ͬ����λ������ͬ���������ʹ�� eSmartCameraCar�����
                ���ʹ��������λ��������Ҫ�޸Ĵ��롣
 *  Sample usage:   sendimg(imgbuff, CAMERA_W * CAMERA_H);                    //���͵���λ��
 */

#define UART_PORT HW_UART4
void sendimg(uint8_t *imgaddr,uint16_t len)
{
 #define CMD_IMG     1
    uint8 cmdf[2] = {CMD_IMG, ~CMD_IMG};    //ɽ����λ�� ʹ�õ�����
    uint8 cmdr[2] = {~CMD_IMG, CMD_IMG};    //ɽ����λ�� ʹ�õ�����
    UART_Send_DATA(UART_PORT, cmdf, 2);      //����֡ͷ����
    UART_Send_DATA(UART_PORT, imgaddr, len); //�ٷ���ͼ��
    UART_Send_DATA(UART_PORT, cmdr, 2);      //����֡β����
}



void sendccd(unsigned char * ImageData) 
{

    unsigned char i;
     char xieyi = 0;
    
    /* Send Data */
    xieyi=0x02;
   UART_WriteByte(UART_PORT,xieyi);
   xieyi=0xFD;
   UART_WriteByte(UART_PORT,xieyi);

    for(i=0; i<128; i++) 
   {
      UART_WriteByte(UART_PORT,*ImageData++);
    }

   UART_WriteByte(UART_PORT,xieyi);
   xieyi=0x02;
   UART_WriteByte(UART_PORT,xieyi);
}


