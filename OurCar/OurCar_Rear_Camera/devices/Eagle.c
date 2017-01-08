#include "common.h"
#include "gpio.h"
#include "dma.h"
#include "UMC.h"
#include "Eagle.h"
#include "sccb.h"
#include "uart.h"
#define OV7725_EAGLE_Delay_ms(time)  DelayMs(time)
#define Thre 30
float Threshold=Thre;//������ֵʹ��

uint8   *ov7725_eagle_img_buff;
volatile IMG_STATUS_e      eagle_img_flag = IMG_FINISH;   //ͼ��״̬


//�ڲ���������
static uint8 ov7725_eagle_reg_init(void);   //ӥ�� �Ĵ��� ��ʼ��
static void ov7725_eagle_port_init(void);			//ӥ�� ���� ��ʼ��
static void Eagle_dma_init(uint8_t chl);
GPIO_InitTypeDef GPIO_InitStruct1;
DMA_InitTypeDef DMA_InitStruct1;

void Eagle_Init(uint8 *imgaddr)
{
  ov7725_eagle_img_buff=imgaddr;
	while(ov7725_eagle_reg_init() == 0);
  ov7725_eagle_port_init();
}

/*!
 *  @brief      ӥ��ov7725�ܽų�ʼ�����ڲ����ã�
 *  @since      v5.0
 */
void ov7725_eagle_port_init(void)
{
		// ���ж� VSYNC ��ʼ��
		PORT_PinMuxConfig(OV7725_VSYNC_PORT, OV7725_VSYNC_PIN, kPinAlt1);
		GPIO_InitStruct1.pinx = OV7725_VSYNC_PIN;
		GPIO_InitStruct1.mode = kGPIO_Mode_IPD;   
		GPIO_InitStruct1.instance = OV7725_VSYNC_PORT;
		GPIO_Init(&GPIO_InitStruct1);
		PORT_PinPassiveFilterConfig(OV7725_VSYNC_PORT, OV7725_VSYNC_PIN, ENABLE);
		GPIOPORT_IRQ_CLEAN(OV7725_VSYNC_PORT);
		PORT_PinPassiveFilterConfig(OV7725_VSYNC_PORT,OV7725_VSYNC_PIN,ENABLE);
		GPIO_ITDMAConfig(OV7725_VSYNC_PORT,OV7725_VSYNC_PIN,kGPIO_IT_RisingEdge,true);
		//ע���жϷ�����
		GPIO_CallbackInstall(OV7725_VSYNC_PORT,Eagle_ISR);
		GPIO_IRQ_DIS(OV7725_VSYNC_PORT);
    //����DMA��ʼ��
    Eagle_dma_init(HW_DMA_CH0);
}

/*!
 *  @brief      ӥ��ov7725���жϷ�����
 *  @since      v5.0
 */
void Eagle_ISR(uint32_t index)
{

	  if((index&(1<<OV7725_VSYNC_PIN)))//���ж�
		{		
			 //UART_printf("VSYNC\n");		
				//���ж���Ҫ�ж��ǳ��������ǳ���ʼ
				if(eagle_img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
				{
						eagle_img_flag = IMG_GATHER;                  //���ͼ��ɼ���
					  // UART_printf("IMG_GATHER\n");
						GPIO_IRQ_DIS(OV7725_VSYNC_PORT);  

						GPIOA_IRQ_CLEAN(OV7725_PCLK_PIN);             //���PCLK��־λ
						DMA_SetDestAddress(HW_DMA_CH0,(uint32)ov7725_eagle_img_buff);    //�ָ���ַ
						DMA_IRQ_CLEAN(HW_DMA_CH0);
						DMA_EnableRequest(HW_DMA_CH0);
						DMA_EN(HW_DMA_CH0);                //ʹ��ͨ��CHn Ӳ������
					
				}
				else                                        //ͼ��ɼ�����
				{
						GPIO_IRQ_DIS(OV7725_VSYNC_PORT);                       //�ر�PTA���ж�
						eagle_img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
					//UART_printf("IMG_FAIL\n");
				}			
		}
} 



void DMA_ISR(void)
{
	    eagle_img_flag = IMG_FINISH ;
	    DMA_IRQ_CLEAN(HW_DMA_CH0);           //���ͨ�������жϱ�־λ
	   // UART_printf("DMAFINISH\n");
	    
}

/*!
 *  @brief      ӥ��ov7725�ɼ�ͼ�񣨲ɼ��������ݴ洢�� ��ʼ��ʱ���õĵ�ַ�ϣ�
 *  @since      v5.0
 */
 void eagle_get_img()
 {
	 //�ȴ���һ��ͼ��ɼ���ɣ���һ�α�������eagle_get_img�ڽ����ڿ�ʼ�ɼ�ͼ���ģ����ڰ������ڲɼ�֮ǰ�����һ��ͼ���Ƿ�ɼ���ɣ�����������߲ɼ�Ч�ʣ���DMA�ɼ�����ѭ������ִ��
	 while(eagle_img_flag != IMG_FINISH)               //�ȴ�ͼ��ɼ����
	 {
		 if(eagle_img_flag == IMG_FAIL)                  //����ͼ��ɼ����������¿�ʼ�ɼ�
		 {
			 eagle_img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
			 GPIOPORT_IRQ_CLEAN(OV7725_VSYNC_PORT);        //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
			 GPIO_IRQ_EN(OV7725_VSYNC_PORT);               //����PTA���ж�
		 }
	 }
	 //��ʼ�ɼ���һ֡ͼ��
	 eagle_img_flag = IMG_START;                 				   //��ʼ�ɼ�ͼ��
	 GPIOPORT_IRQ_CLEAN(OV7725_VSYNC_PORT);                //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
	 GPIO_IRQ_EN(OV7725_VSYNC_PORT);                       //����PTA���ж�
 }



/*OV7725��ʼ�����ñ�*/
reg_s ov7725_eagle_reg[] =
{
    //�Ĵ������Ĵ���ֵ��
    {OV7725_COM4         , 0xC1},
    {OV7725_CLKRC        , 0x02},//50hz
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},
    {OV7725_COM7         , 0x40},
    {OV7725_HSTART       , 0x3F},
    {OV7725_HSIZE        , 0x50},
    {OV7725_VSTRT        , 0x03},
    {OV7725_VSIZE        , 0x78},
    {OV7725_HREF         , 0x00},
    {OV7725_SCAL0        , 0x0A},
    {OV7725_AWB_Ctrl0    , 0xE0},
    {OV7725_DSPAuto      , 0xff},
    {OV7725_DSP_Ctrl2    , 0x0C},
    {OV7725_DSP_Ctrl3    , 0x00},
    {OV7725_DSP_Ctrl4    , 0x00},

#if (OV7725_W == 80)
    {OV7725_HOutSize     , 0x14},
#elif (OV7725_W == 160)
    {OV7725_HOutSize     , 0x28},
#elif (OV7725_W == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (OV7725_W == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (OV7725_H == 60 )
    {OV7725_VOutSize     , 0x1E},
#elif (OV7725_H == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (OV7725_H == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (OV7725_H == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

    {OV7725_EXHCH        , 0x00},
    {OV7725_GAM1         , 0x0c},
    {OV7725_GAM2         , 0x16},
    {OV7725_GAM3         , 0x2a},
    {OV7725_GAM4         , 0x4e},
    {OV7725_GAM5         , 0x61},
    {OV7725_GAM6         , 0x6f},
    {OV7725_GAM7         , 0x7b},
    {OV7725_GAM8         , 0x86},
    {OV7725_GAM9         , 0x8e},
    {OV7725_GAM10        , 0x97},
    {OV7725_GAM11        , 0xa4},
    {OV7725_GAM12        , 0xaf},
    {OV7725_GAM13        , 0xc5},
    {OV7725_GAM14        , 0xd7},
    {OV7725_GAM15        , 0xe8},
    {OV7725_SLOP         , 0x20},
    {OV7725_LC_RADI      , 0x00},
    {OV7725_LC_COEF      , 0x13},
    {OV7725_LC_XC        , 0x08},
    {OV7725_LC_COEFB     , 0x14},
    {OV7725_LC_COEFR     , 0x17},
    {OV7725_LC_CTR       , 0x05},
    {OV7725_BDBase       , 0x99},
    {OV7725_BDMStep      , 0x03},
    {OV7725_SDE          , 0x04},
    {OV7725_BRIGHT       , 0x00},
    {OV7725_CNST         , Thre},
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},

};



 
void Eagle_dma_init(uint8_t chl)
{
	    uint8_t i=0;
			/*  ��������Դ   */
			//����IO�ڳ�ʼ��
			for(i=0;i<8;i++)
			{
				PORT_PinMuxConfig(HW_GPIOB, i, kPinAlt1);
				GPIO_InitStruct1.pinx = i;
				GPIO_InitStruct1.mode = kGPIO_Mode_IPD;
				GPIO_InitStruct1.instance = HW_GPIOB;
				GPIO_Init(&GPIO_InitStruct1);
			}
	    //���ô���Դ��Ĭ���� �����ش�����
			//�����ж� PCLK ��ʼ��
			PORT_PinMuxConfig(OV7725_PCLK_PORT, OV7725_PCLK_PIN, kPinAlt1);
			GPIO_InitStruct1.pinx = OV7725_PCLK_PIN;
			GPIO_InitStruct1.mode = kGPIO_Mode_IPU;
			GPIO_InitStruct1.instance = OV7725_PCLK_PORT;
			GPIO_Init(&GPIO_InitStruct1);
			GPIO_ITDMAConfig(OV7725_PCLK_PORT,OV7725_PCLK_PIN,kGPIO_DMA_FallingEdge,true);
			//DMA �Ĵ��� ����
			DMA_DIS(chl);     //ʧ��ͨ��CHn Ӳ������
			/* clear some register */
			DMA0->TCD[chl].ATTR  = 0;
			DMA0->TCD[chl].CSR   = 0;
			/* ����ʱ�� */
			SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;                        //��DMAģ��ʱ��
			SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;                     //��DMA��·������ʱ��
			/* ���� DMA ͨ�� �� ������ƿ� TCD ( Transfer Control Descriptor ) */
			DMA0->TCD[chl].SADDR =  (uint32_t)&PTB->PDIR;                      // ����  Դ��ַ
			DMA0->TCD[chl].DADDR =  (uint32_t)ov7725_eagle_img_buff;          // ����Ŀ�ĵ�ַ
			DMA0->TCD[chl].SOFF  =    0;                              // ����Դ��ַƫ�� = 0x0, ������
			DMA0->TCD[chl].DOFF  =    1;                              // ÿ�δ����Ŀ�ĵ�ַ�� BYTEs
			DMA0->TCD[chl].ATTR  =    (0
													 | DMA_ATTR_SMOD(0x0u)               // Դ��ַģ����ֹ  Source address modulo feature is disabled
													 | DMA_ATTR_SSIZE(0)            // Դ����λ�� ��DMA_BYTEn  ��    SSIZE = 0 -> 8-bit ��SSIZE = 1 -> 16-bit ��SSIZE = 2 -> 32-bit ��SSIZE = 4 -> 16-byte
													 | DMA_ATTR_DMOD(0x0u)               // Ŀ���ַģ����ֹ
													 | DMA_ATTR_DSIZE(0)            // Ŀ������λ�� ��DMA_BYTEn  ��  ���òο�  SSIZE
													);
			DMA0->TCD[chl].CITER_ELINKNO  = DMA_CITER_ELINKNO_CITER(OV7725_W*OV7725_H/8);   //��ǰ��ѭ������
			DMA0->TCD[chl].BITER_ELINKNO  = DMA_BITER_ELINKNO_BITER(OV7725_W*OV7725_H/8);   //��ʼ��ѭ������
			DMA0->CR &= ~DMA_CR_EMLM_MASK;                                // CR[EMLM] = 0
			//��CR[EMLM] = 0 ʱ:
			DMA0->TCD[chl].NBYTES_MLNO =   DMA_NBYTES_MLNO_NBYTES(1); // ͨ��ÿ�δ����ֽ�������������ΪBYTEs���ֽڡ�ע��ֵΪ0��ʾ����4GB */
			//ע���жϷ�����
			DMA_CallbackInstall(HW_DMA_CH0,DMA_ISR);
			/* ���� DMA ���������Ĳ��� */
			DMA0->TCD[chl].SLAST      =   0;                              //����  Դ��ַ�ĸ���ֵ,��ѭ��������ָ�  Դ��ַ
			DMA0->TCD[chl].DLAST_SGA  =   (uint32)(0); //����Ŀ�ĵ�ַ�ĸ���ֵ,��ѭ��������ָ�Ŀ�ĵ�ַ���߱��ֵ�ַ//����ԭ��ַ
			DMA0->TCD[chl].CSR        =   (0
															 | DMA_CSR_BWC(3)               //�������,ÿ��һ�Σ�eDMA ����ֹͣ 8 �����ڣ�0��ֹͣ��1������2ֹͣ4���ڣ�3ֹͣ8���ڣ�
															 | DMA_CSR_DREQ_MASK            //��ѭ��������ֹͣӲ������
															 | DMA_CSR_INTMAJOR_MASK        //��ѭ������������ж�
															);

			/* ���� DMA ����Դ */
			DMAMUX_InstanceTable[0]->CHCFG[chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
			DMAMUX_InstanceTable[0]->CHCFG[chl] = (0
																									 | DMAMUX_CHCFG_ENBL_MASK                        /* Enable routing of DMA request */
																									 //| DMAMUX_CHCFG_TRIG_MASK                        /* Trigger Mode: Periodic   PIT���ڴ�������ģʽ   ͨ��1��ӦPIT1������ʹ��PIT1����������Ӧ��PIT��ʱ���� */
																									 | DMAMUX_CHCFG_SOURCE( PORTA_DMAREQ) /* ͨ����������Դ:     */
																							 );									
			/* enable DMAMUX */
	  	DMAMUX_InstanceTable[0]->CHCFG[chl] |= DMAMUX_CHCFG_ENBL_MASK;
			DMA_IRQ_CLEAN(chl);
			/* �����ж� */
			DMA_EN(chl);                                    //ʹ��ͨ��CHn Ӳ������
			DMA_IRQ_EN(chl);                                //����DMAͨ������
}




uint8 ov7725_eagle_cfgnum = ARR_SIZE( ov7725_eagle_reg ) ; /*�ṹ�������Ա��Ŀ*/





/*!
 *  @brief      ӥ��ov7725�Ĵ��� ��ʼ��
 *  @return     ��ʼ�������0��ʾʧ�ܣ�1��ʾ�ɹ���
 *  @since      v5.0
 */
uint8 ov7725_eagle_reg_init(void)
{
    uint16 i = 0;
    uint8 Sensor_IDCode = 0;
    SCCB_GPIO_init();
    OV7725_EAGLE_Delay_ms(50);
    if( 0 == SCCB_WriteByte ( OV7725_COM7, 0x80 ) ) /*��λsensor */
    {
        //UART_printf("����:SCCBд���ݴ���\n");
        return 0 ;
    }

    OV7725_EAGLE_Delay_ms(50);

    if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, OV7725_VER ) )    /* ��ȡsensor ID��*/
    {
        //UART_printf("����:��ȡIDʧ��\n");
        return 0;
    }
    //UART_printf("Get ID success,SENSOR ID is 0x%x\n", Sensor_IDCode);
    //UART_printf("Config Register Number is %d \n" ,ov7725_eagle_cfgnum);
    if(Sensor_IDCode == OV7725_ID)
    {
        for( i = 0 ; i<ov7725_eagle_cfgnum ; i++ )
        {
            if( 0 == SCCB_WriteByte(ov7725_eagle_reg[i].addr, ov7725_eagle_reg[i].val) )
            {
                //UART_printf("����:д�Ĵ���0x%xʧ��\n", ov7725_eagle_reg[i].addr);
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
    //UART_printf("OV7725 Register Config Success!\n");
    return 1;
}





//ѹ����ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
//srclen �Ƕ�ֵ��ͼ���ռ�ÿռ��С
void img_extract(uint8_t *dst, uint8_t *src, uint32_t srclen)
{
    uint8_t colour[2] = {255, 0}; //0 �� 1 �ֱ��Ӧ����ɫ
    //ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
    uint8_t tmpsrc;
		
		
		
		
		
		
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
    }
		

}


