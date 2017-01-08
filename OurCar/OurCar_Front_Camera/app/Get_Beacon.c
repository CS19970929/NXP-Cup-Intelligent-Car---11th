#include "common.h"
#include "servo.h"
#include "motor.h"
#include "Get_Beacon.h"

extern uint8_t pic[60][80];//��ѹ���ͼ������
extern uint8_t H_Mid_line_flag;//������ͷ�ű���ձ�־
extern  uint8_t F_Mid_line_flag;

long int abs_long_int(long int dat)
{
	return dat>0?dat:-dat;
}

//ͼ���ֵ��ͼ�������������������������img0��
//img0 img1  �� ������ֵ��ͼ�񣬼�һ��bit��ʾһ������
//Pixelcnt �� ͼ����ֽ���
void img_XOR(uint8_t * img0,uint8_t * img1,uint16_t Pixelcnt)
{
	uint16_t i=0;
	for(i=0;i<Pixelcnt;i++)
	{
		*(img0+i)^=*(img1+i);
	}
}

/// ���ݷ������ͨ��
//x :          �õ�ĺ�����
//y :          �õ��������
//isMarked :   �õ��Ƿ��Ѿ�����ǹ������ڼ�¼����·�ߡ�δ���Ϊ0������õ��Ѿ�����ǹ�����Ӧָ���ò���Ϊ1
//���������ж�̫�࣬���Կ���д���е��Σ�Ҳ����Щ����ǲ����ܷ����ģ����������������ж�
//������������Bug������Ч������Ҫ�󣡣���

#define q 60                     //��ͬ��ͨ��Ҷ�ֵ��࣬��ֻ��Ϊ�˰ѱ�ǵ���ͨ�򷢻���λ���鿴���Ч��
uint8_t mark=0;                     //����ǵĸ���������ͨ����
uint8_t mat[60][80]={0};            //��ʱ�ѱ����ͨ��ͼ����������ʵû��Ҫ������һ��ռ�
extern uint8_t pic[60][80];				//��ѹ���ֵ��ͼ��1 byte--->1 pixel 
uint8_t Back_Flag=0;                //���ݱ�־�������˼�¼��ͨ����Ŀʱ����mark�ļ�����

float X_Start = 11;//ͼ����ʼ�У���Ҫ��Ϊ�˱�����ߵĸ��ţ�ͼ���Ϸ��Ǹ�����������

void Connect(int x, int y, uint8_t isMarked)
{
	if (x == (uint8_t)X_Start && y == 0) //mat[0, 0]
	{
		//��һ�ν��븴λmark������ͻ��ݱ�־
		mark=0;
		Back_Flag=0;
		if (pic[x][y] != 0 )
		{			
			mat[x][y] = (++mark)*q+1; // �µ���ͨ��
		}
	}else if (x != (uint8_t)X_Start && y == 0) // 
	{
		if (pic[x][y] != 0)
		{
			if (mat[x - 1][ y] != 0)
			{
				if(isMarked ==0)
				{
					mat[x][y] = mat[x - 1][y]; 
				}else
				{
					if(mat[x-1][y] > mat[x][y])
					{
						mat[x-1][y] = mat[x][y]; // up one
						Connect(x - 1, y, 1);
					}
				}
			}
			else
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)   //δ�������֮ǰ���ݹ���˵����һ�ֻ�����ɣ��Ա����mark����һ����
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = (++mark)*q+1; // �µ���ͨ��
				}
			}
		}
	}else if (x == (uint8_t)X_Start && y != 0) // First Column
	{
		if (pic[x][y] != 0)
		{
			if (mat[x][ y-1] != 0)
			{
				if(isMarked == 0)
				{
					mat[x][y] = mat[x][ y-1]; 
				}else
				{
					if(mat[x][ y-1]>mat[x][ y])
					{
						mat[x][y-1] = mat[x][ y]; // left one
						Connect(x, y - 1, 1);	
					}
				}
			}
			else
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = (++mark)*q+1; // �µ���ͨ��
				}
			}
		}
	}else if (x != (uint8_t)X_Start && y != 0) // other pixel
	{
		if (pic[x][y] != 0)
		{
			if (mat[x][ y-1] == 0 && mat[x - 1][ y] == 0)
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = (++mark)*q+1;   // �µ���ͨ��
				}
			}
			else if (mat[x][ y-1] == 0 && mat[x - 1][ y] != 0)
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = mat[x - 1][y];
				}
				else
				{
					if (mat[x - 1][ y] > mat[x][ y])
					{
					mat[x - 1][ y] = mat[x ][y];
					Connect(x - 1, y, 1); // ��x�����������
					}
				}
			}
			else if (mat[x][ y-1] != 0 && mat[x - 1][ y] == 0)
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = mat[x][ y-1];
				}
				else
				{
					if (mat[x][ y-1] > mat[x][ y])
					{
					mat[x][ y-1] = mat[x ][y];
					Connect(x, y - 1, 1); // ��y�����������
					}
				}
			}
			else if (mat[x][ y-1] != 0 && mat[x - 1][ y] != 0 && mat[x][ y-1] == mat[x - 1][y])
			{
				if (isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					mat[x][y] = mat[x][ y-1];
				}
				else
				{
					if (mat[x][ y-1] > mat[x][ y])
					{
							mat[x][ y-1] = mat[x - 1][ y] = mat[x ][y];
							Connect(x - 1, y, 1); // �����ϱߺ���߶����ѱ�����ص����������ͬʱ����
							Connect(x, y - 1, 1);
					}
				}
			}
			else if (mat[x][ y-1] != 0 && mat[x - 1][ y] != 0 && mat[x][ y-1] != mat[x - 1][y])
			{
				if(isMarked == 0)
				{
					if(Back_Flag==1)
					{
						mark--;
						Back_Flag=0;
					}
					if(mat[x - 1][ y]>mat[x][ y - 1])
					{
						mat[x - 1][ y] = mat[x][y] = mat[x][ y - 1];
						Connect(x - 1, y, 1);
						if(mat[x-1][ y]!=mat[x-1][ y-1])
						Back_Flag=1;
					}else if(mat[x - 1][ y]<mat[x][ y - 1])
					{
						mat[x][ y-1] = mat[x][y] = mat[x-1][ y];
						Connect(x, y - 1, 1);
						if(mat[x-1][ y]!=mat[x-1][ y-1])
						Back_Flag=1;
					}
	  		}else
				{
					if(mat[x - 1][ y]>mat[x][y])
					{
						mat[x - 1][ y] = mat[x][y];
						Connect(x - 1, y, 1);
					}
					if(mat[x ][ y-1] > mat[x][y])
					{
						mat[x ][ y-1] = mat[x][y];
						Connect(x , y-1, 1);
					}
				}
			}
		}
	}					
}
		
#define x1 0       //��ͨ���ǣ�X���
#define x2 1       //��ͨ���ǣ�X�յ�
#define y1 2       //��ͨ���ǣ�Y���
#define y2 3       //��ͨ���ǣ�Y�յ�

//Aspect ratio						
float Ar_max=1.5;//1.33;         //��߱����ֵ
float Ar_min=0.5;//0.77;         //��߱���Сֵ
//White ratio
float Wr_min = 0.5;//0.7; //��ɫ����ռ���εı�����Сֵ   0.785;pi/4
 
#define Connected_domain_cnt 20
uint8_t Connected_domain[Connected_domain_cnt][4]={0};//��ͨ������ֵ��¼
uint16_t White_cnt[Connected_domain_cnt] = {0};
float White_Ratio=0.0;   //��ɫ����ռ���εı���
float W[5]={0.0};        //��ͨ���
float H[5]={0.0};        //��ͨ���
float WH_Ratio=0.0; //��ͨ���߱�
float White_Min = 3;//ͼ������ֵ�Ƚϴ��ʱ�����һЩ��ɫ��㣬���ư�ɫ��ͨ����С��ɫ���ص�����˳������š�
void GetBeacon(uint8_t Mat[60][80],struct Beacon * DstBeacon)
{
	F_Mid_line_flag=0;
	int16 xx=0;
	int16 yy=0;
	uint16_t White_cnt_max=White_Min;
  for(int16 xx=(uint8_t)X_Start;xx<60;xx++)
 {
		for(int16 yy=0;yy<80;yy++)
		{
			 mat[xx][yy]=0;
		}
	}
	for(xx=(uint8_t)X_Start;xx<60;xx++)
  {
	  for(yy=0;yy<80;yy++)
	  {
		  Connect(xx,yy,0);
  	}
  }
	//��ͨ���ǡ���ɫ���������λ
	for(xx=0;xx<Connected_domain_cnt;xx++)
	{
		Connected_domain[xx][x1] = 255;
		Connected_domain[xx][x2] = 0;
		Connected_domain[xx][y1] = 255;
		Connected_domain[xx][y2] = 0;
		White_cnt[xx] = 0;
	}
	//��ͨ����࣬��Ϊ�Ǹ��ţ��˳��������
	if(mark>=Connected_domain_cnt)
		return;
	//�����ͨ��Ŀǰ�Ǳ������д��Ľ�
	for(xx=(uint8_t)X_Start;xx<60;xx++)
	{
		for(yy=0;yy<80;yy++)
		{
			 if(Mat[xx][yy]!=0)
			 {
				 White_cnt[(Mat[xx][yy]-1)/q]++;
				 if(xx< Connected_domain[(Mat[xx][yy]-1)/q][x1])
				 {
					 Connected_domain[(Mat[xx][yy]-1)/q][x1] = xx;
				 }
				 if(xx> Connected_domain[(Mat[xx][yy]-1)/q][x2])
				 {
					 Connected_domain[(Mat[xx][yy]-1)/q][x2] = xx;
				 }
				 if(yy< Connected_domain[(Mat[xx][yy]-1)/q][y1])
				 {
					 Connected_domain[(Mat[xx][yy]-1)/q][y1] = yy;
				 }
				 if(yy> Connected_domain[(Mat[xx][yy]-1)/q][y2])
				 {
					 Connected_domain[(Mat[xx][yy]-1)/q][y2] = yy;
				 }
			 }
		 }
	 }
	//������ͨ���߱ȣ�
	for(int16 marknum=1;marknum<=mark;marknum++)
	{
		 H[marknum-1] =  (float)(Connected_domain[marknum][x2]-Connected_domain[marknum][x1])+1;
		 W[marknum-1] =  (float)(Connected_domain[marknum][y2]-Connected_domain[marknum][y1])+1;
		 WH_Ratio=W[marknum-1]/H[marknum-1];
		 White_Ratio = (float)White_cnt[marknum]/(H[marknum-1]*W[marknum-1]);
		 if(WH_Ratio<Ar_max&& WH_Ratio>Ar_min && White_Ratio>Wr_min)
		 {
			 //ȡ�׵�������ͨ��Ϊ�ű꣬�д��Ľ�
			 if(White_cnt[marknum]>White_cnt_max&&White_cnt[marknum]>5)
			 {
				 White_cnt_max = White_cnt[marknum];
				 DstBeacon->x = (Connected_domain[marknum][x2]+Connected_domain[marknum][x1])/2;
				 DstBeacon->y = (Connected_domain[marknum][y2]+Connected_domain[marknum][y1])/2;
				 F_Mid_line_flag=1;
			 }
			 uint8_t temp = marknum*q+1;
			 //���Ǳ������д��Ľ�
			 for(xx=(uint8_t)X_Start;xx<60;xx++)
				{
					for(yy=0;yy<80;yy++)
					{
						 if(Mat[xx][yy]==temp)
						 {
							 Mat[xx][yy]=255;
						 }
					 }
				 }
		 }
	 }     
}			


uint8_t Beacon_cnt = 0;//��¼�ڼ����ű�
uint8_t Pre_Beacon_cnt = 0;
uint8_t Beaco_cnt_flag = 1;
float Cnt_X_Thre = 30;
float Cnt_Max=3.0;
extern struct Beacon DstBeacon;//Ŀ���ű�λ��
/*********************************************
* �ű��������
*********************************************/
void Beacon_CNT_Update(void)
{
	Beacon_cnt=Pre_Beacon_cnt;
}
/*********************************************
* �ű����
*********************************************/
void Beacon_CNT(void)
{
	if(DstBeacon.x>Cnt_X_Thre)
	{
		if(Beaco_cnt_flag>(uint8_t)Cnt_Max)
		{
			Pre_Beacon_cnt++;
		}
		Beaco_cnt_flag=0;	
	}else
	{
		Beaco_cnt_flag++;
		if(Beaco_cnt_flag>(uint8_t)Cnt_Max)
		{
			Beaco_cnt_flag=(uint8_t)(Cnt_Max+1);
			Beacon_CNT_Update();
		}
	}
}





					



