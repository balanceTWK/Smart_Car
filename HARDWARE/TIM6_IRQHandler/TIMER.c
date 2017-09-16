#include "TIMER.h"
#include "usart.h"
#include "ADXL345_IIC.h"
#include "HMC5883.h"
#include  "math.h"

short int x,y,z;
double angle;
u16 Task_Delay[NumOfTask];
extern short int ADXL345Buff[3];
extern short int Hmc5883Buff[3];
extern u16 speed1,speed2,speed3,speed4;
extern int  pwm1,pwm2,pwm3,pwm4; 
extern u8 signal;
void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM6��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}
//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)   //TIM6�ж�
{
	u8 i;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־
//printf("b��%d c��%d pwm1��%d pwm2��%d pwm3��%d pwm4��%d speed1��%d  speed2��%d  speed3��%d  speed4��%d  \r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4,speed1,speed2,speed3,speed4);
//printf("b��%d c��%d pwm1��%d pwm2��%d pwm3��%d pwm4��%d\r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4);
// printf("speed1��%d speed2��%d speed3��%d speed4��%d\r\n",
//		     speed1,speed2,speed3,speed4);
			for(i=0;i<NumOfTask;i++)
			{
				if(Task_Delay[i])
				{
					Task_Delay[i]--;
				}
			}
		}
}


void TIM6_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM6��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM6, ENABLE);  //ʹ��TIMx					 
}
//��ʱ��2�жϷ������
void TIM6_IRQHandler(void)   //TIM6�ж�
{
//	static u8 count1=0;
//	static u8 count2=0;
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM6�����жϷ������
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־
//		  Multiple_read_ADXL345();
//			Multiple_read_Hmc5883();
			
//		x=Hmc5883Buff[0];
//		y=Hmc5883Buff[1];
//		z=Hmc5883Buff[2];

//		angle= atan2((double)y,(double)x) * (180 / 3.14159265) + 180;
//			
//			if(ADXL345Buff[1]>60)
//			{
//				count1++;
//				if(count1>=7){signal&=0xF9;signal|=0x02;count1=0;}
//			}
//			else if(ADXL345Buff[1]<-80)
//			{
//				count2++;
//				if(count2>=7){signal&=0xF9;signal|=0x04;count1=0;}
//			}
//			else {count1=0;count1=0;}
			
//printf("b��%d c��%d pwm1��%d pwm2��%d pwm3��%d pwm4��%d speed1��%d  speed2��%d  speed3��%d  speed4��%d  \r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4,speed1,speed2,speed3,speed4);
//printf("b��%d c��%d pwm1��%d pwm2��%d pwm3��%d pwm4��%d\r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4);
// printf("speed1��%d speed2��%d speed3��%d speed4��%d\r\n",
//		     speed1,speed2,speed3,speed4);
//			printf("signal %d x : %d y : %d  z : %d   angle: %f\r\n",
//		     signal,ADXL345Buff[0],ADXL345Buff[1],ADXL345Buff[2],angle);
//			printf(" angle: %f\r\n",angle);
//			printf(" Z: %d\r\n",Hmc5883Buff[2]);
		}
}



