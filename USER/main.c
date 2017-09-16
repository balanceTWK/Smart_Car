#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "ADXL345_IIC.h"
#include "bibi.h"
#include "key.h"
#include "led.h"
#include "motor.h"
#include "OLED_I2C.h"
#include "PWM.h"
#include "seek.h"
#include "SPEED.h"
#include "TIMER.h"
#include "HMC5883.h"

/************************************************
 STM32F103ZET6����
 ���ߣ���ΰ��
************************************************/

int i;//�жദ���ã�ע��ÿ��ʹ�ú�Ҫ����,���´ε��ó���.

extern short int ADXL345Buff[3];//������ٶȼ�����xyz��
extern u16 Task_Delay[NumOfTask];//NumOfTask��ʾÿ���׶��µĲ���С�������.
extern u8 state;//1Ϊǰ��,0Ϊ����,3Ϊ˳ʱ��  ��motor.c
u16 v1,v2,v3,v4;//С�������ٶ�
u16 speed1,speed2,speed3,speed4;//С��ʵʱ�ٶ�
int  pwm1=0,pwm2=0,pwm3=0,pwm4=0; //С�����PWMֵ
u8 key;//����ɨ��
volatile u8 backseek,seek;//ѭ������

u8 signal=0;//�ж�С��ƽ��״̬����һλ��ʾ�Ӽ��ٶ�,�ڶ�λ��ʾ����,����λ��ʾ���£��ñ����� TIMER.C�����ã�
void GetSpeed()
{
		speed1=(TIM3->CNT);
		speed2=(TIM4->CNT);
		speed3=(TIM5->CNT);
		speed4=(TIM8->CNT);
	 if(state==1)//�ж�Ϊǰ��
	 {
	 	speed3=65536-speed3;
		speed4=65536-speed4;
	 }
	 else if(state==2)//�ж�Ϊ����
	 {
	 	speed1=65536-speed1;
		speed2=65536-speed2;
	 }
	 else if(state==3)//�ж�Ϊ˳ʱ��תȦ
	 {
    LED0=~LED0;
	 }	 
	 else if(state==5)
	 {
		speed3=65536-speed3;
		speed4=65536-speed4;
	 	speed1=65536-speed1;
		speed2=65536-speed2;
	 }
		TIM3->CNT = 0;//��������������
		TIM5->CNT = 0;
		TIM4->CNT = 0;
		TIM8->CNT = 0;
}

int GetSpeedNoZero()
{
	int count;
		speed1=(TIM3->CNT);
		speed2=(TIM4->CNT);
		speed3=(TIM5->CNT);
		speed4=(TIM8->CNT);
	 if(state==1)//�ж�Ϊǰ��
	 {
	 	speed3=65536-speed3;
		speed4=65536-speed4;
	 }
	 else if(state==2)//�ж�Ϊ����
	 {
	 	speed1=65536-speed1;
		speed2=65536-speed2;
	 }
	 else if(state==3)//�ж�Ϊ˳ʱ��תȦ
	 {
    LED0=~LED0;
	 }	 
	 count=speed1+speed2+speed3+speed4;
	 return count;
}


void PID(int sv1,int sv2,int sv3,int sv4)//�������ô˺��� ÿ�ε��ñ������ʱ������ʱ������50msΪ��ѡ�
{
//	int error;
	  GetSpeed();
		sv1+=3;//���У��
		sv2+=3;//���У��
	
//	error=(speed1+speed2+speed3+speed4-sv1-sv2-sv3-sv4);
//	if((error>200)||(error<-200))signal|=0x01;
//	else signal&=0xFE;
	
		if(speed1>sv1){pwm1=(int16_t)(pwm1+4*(speed1-sv1));}      //�ջ�����
		else if(sv1>speed1){(int16_t)(pwm1=pwm1-4*(sv1-speed1));} //�ջ�����
		if(pwm1>=6000)pwm1=6000;                          
		if(pwm1<=0)pwm1=0;                                  
		
		if(speed2>sv2){pwm2=(int16_t)(pwm2+4*(speed2-sv2));}      //�ջ�����
		else if(sv2>speed2){(int16_t)(pwm2=pwm2-4*(sv2-speed2));} //�ջ�����
		if(pwm2>=6000)pwm2=6000;                             
		if(pwm2<=0)pwm2=0;                                  
		
		if(speed3>sv3){pwm3=(int16_t)(pwm3+4*(speed3-sv3));}      //�ջ�����
		else if(sv3>speed3){(int16_t)(pwm3=pwm3-4*(sv3-speed3));} //�ջ�����
		if(pwm3>=6000)pwm3=6000;                            
		if(pwm3<=0)pwm3=0;                                  
		
		if(speed4>sv4){pwm4=(int16_t)(pwm4+4*(speed4-sv4));}      //�ջ�����
		else if(sv4>speed4){(int16_t)(pwm4=pwm4-4*(sv4-speed4));} //�ջ�����
		if(pwm4>=6000)pwm4=6000;                              
		if(pwm4<=0)pwm4=0;  
		
		TIM1->CCR1=pwm1;
		TIM1->CCR2=pwm2;
		TIM1->CCR3=pwm3;
		TIM1->CCR4=pwm4;
		
}

void ChangePwm(int p1,int p2,int p3,int p4)
{
		TIM1->CCR1=p1;
		TIM1->CCR2=p2;
		TIM1->CCR3=p3;
		TIM1->CCR4=p4;
}

void SeekADJ()
{
	seek=~Recive_Seek();//ȡ����seek ����Ϊ1 ����Ϊ0;
	
			  if((seek&0x18)==0x18){GO_Forward();PID(400,400,400,400);}//0001 1000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ��� 
			  else if (seek&0x08){GO_Forward();ChangePwm(2268,2183,1560,1632);}//0001 0000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ���
						 else if(seek&0x10){GO_Forward();ChangePwm(1577,1487,2495,2767);} //0000 1000
	                else if(seek&0x04){car_leftStop();ChangePwm(2805,2553,1560,1632);}//0010 0000
	                     else if(seek&0x20){car_rightStop();ChangePwm(1577,1487,3205,3323);}// 0000 0100
														else if(seek&0x02){car_leftStop();ChangePwm(3258,2933,1192,1267);}//0100 0000
																 else if(seek&0x40){car_rightStop();ChangePwm(1204,1124,3893,3945);}//0000 0010
															        else if(seek&0x01){car_leftStop();ChangePwm(3458,3133,792,967);}//1000 0000
																           else if(seek&0x80){car_rightStop();ChangePwm(904,824,4093,4145);}//0000 0001

	
}
void ADJ()
{
	seek=~Recive_Seek();//ȡ����seek ����Ϊ1 ����Ϊ0;
	
////			  if((seek&0x18)==0x18){GO_Forward();PID(400,400,400,400);}//0001 1000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ��� 
//			  else 
if (seek&0x08){car_leftStop();}//0001 0000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ���
						 else if(seek&0x10){car_rightStop();} //0000 1000
	                else if(seek&0x04){car_leftStop();}//0010 0000
	                     else if(seek&0x20){car_rightStop();}// 0000 0100
														else if(seek&0x02){car_leftStop();}//0100 0000
																 else if(seek&0x40){car_rightStop();}//0000 0010
															        else if(seek&0x01){car_leftStop();}//1000 0000
																           else if(seek&0x80){car_rightStop();}//0000 0001

	
}

void SlowSeekADJ()
{
	seek=~Recive_Seek();//ȡ����seek ����Ϊ1 ����Ϊ0;
	
			  if((seek&0x18)==0x18)PID(100,100,100,100);//0001 1000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ���
			  else if (seek&0x08)ChangePwm(3258,2933,2933,2495);//0001 0000  �����������Ƿ�װ�ĵ�,���ж�Ҳ�Ƿ���
						 else if(seek&0x10)ChangePwm(2268,2183,3893,3945); //0000 1000
	                else if(seek&0x04)ChangePwm(3568,3233,2633,2195);//0010 0000
	                     else if(seek&0x20)ChangePwm(1968,1883,4193,4245);// 0000 0100
														else if(seek&0x02)ChangePwm(3868,3533,2333,1895);//0100 0000
																 else if(seek&0x40)ChangePwm(1668,1583,4493,4545);//0000 0010
															        else if(seek&0x01)ChangePwm(4168,3833,2033,1595);//1000 0000
																           else if(seek&0x80)ChangePwm(1368,1283,4793,4845);//0000 0001
//	                                              else {PID(100,100,100,100);}
	
}

//u8 filter(u8 i)
//{
//	u8 n;
//	u8 m=0;
//	for(n=0;n<8;n++)
//	{
//		if(i&0x01){m++;}
//		i=(i>>1);
//	}
//	return m;
//}

int main(void)
{	
	
  delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	BigLED_Init();//��������ʼ��
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	I2C_Configuration();
	OLED_Init();	
	Init_ADXL345();//������ٶȼƳ�ʼ��
	uart_init(115200);    //���ڲ���������
	LED_Init();         //LED��ʼ��
	PWM_Init(7199,0);//PWM ����Ƶ��PWMƵ��=72000/(7199+1)=10Khz��
	SPEED_Init(0xFFFF,0);		//��72Mhz��Ƶ�ʼ��� 0��0
	Motor_Init();///////////////////////////////////////////////////////////////////////////////////
	Car_Stop();
	KEY_Init();
	SEEK_Init();
	Init_Hmc5883();//�شż�
	OLED_Fill(0xFF);//ȫ������
	delay_ms(200);
	OLED_Fill(0x00);//ȫ����	
	
//	TIM6_Int_Init(2999,7199);//====30ms��һ���жϷ�����,�жϷ�������TIMER.c   ���ʱ�䣨��λΪ us��= ((arr+1)*(psc+1))/72��
	TIM2_Int_Init(49,7199);//=====10MS��һ���жϷ��������жϷ�������TIMER.c
	delay_ms(20);
	
  while(1)
	{
		
		while(1)                                                             /*��ʼ����,�ȴ�����*/
		{
			Car_Stop();
			if(Bhend==0)
			{
			delay_ms(20);
       if(Bhend==0)break;
			}
	     key=KEY_Scan(0);
		   if(key==KEY0_PRES)//���п���С����
			  {
			    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
					while(1){LED0=~LED0; LED1=~LED0;delay_ms(1000);}
			  }			
		}
		BIBI=1;
		while(1)                                                                     /*�ջ������ٶ�һ��ʱ��,ʹ�ٶ��ȶ�.*/
		{
		    if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					i++;
					
					LED0=~LED0;
				  GO_Forward();
  				PID(300,300,300,300);        				
					Task_Delay[0]=10;
					if(i>=10){i=0;TIM2->CR1 |= TIM_CR1_CEN;break;}
					TIM2->CR1 |= TIM_CR1_CEN; 
          
				}
				
		}
		
		while(1)                                                                      
		{	
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					i++;
					SeekADJ();
					
					Task_Delay[0]=10; 
          if(i>=15){i=0;TIM2->CR1 |= TIM_CR1_CEN;break;}
					TIM2->CR1 |= TIM_CR1_CEN;
           
				}			
					
		}
		BIBI=0;
		while(1)                                                                        /*Ѱ�߲��ж��Ƿ�Ҫ�ϵڶ���ƽ̨*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					//////////////////////////////////**/jiajiajiajiajaijaiajiajijaijaijaiajiajaijaiajiajai
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}	
				
				if(LeftCollide==1||RightCollide==1)break;	
		}
		TIM2->CR1 &=~TIM_CR1_CEN;
   ChangePwm(1577,1487,1560,1632);
	 delay_ms(800);

	 GO_Back();
	 delay_ms(100);
	 Car_Stop();
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 delay_ms(500);
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 ChangePwm(1937,1863,1753,1876);
	 R_Circle();
	 delay_ms(500);
	 delay_ms(200);
	 TIM2->CR1 |= TIM_CR1_CEN;
	 while(1)
	 {
	  if(Laser==1)break;
	 
	 }

		TIM3->CNT = 100;//������������ƭ
		TIM5->CNT = 100;
		TIM4->CNT = 100;
		TIM8->CNT = 100;
    GO_Forward();
		BIBI=1;
		while(1)                                                                     /*�ջ������ٶ�һ��ʱ��,ʹ�ٶ��ȶ�.*/
		{
		    if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					LED0=~LED0;
				  GO_Forward();
  				PID(200,200,200,200);        
					
					Task_Delay[0]=10; 
					TIM2->CR1 |= TIM_CR1_CEN;
				}
				if(RightCollide==1||LeftCollide==1)break;	
		}
		BIBI=0;
		while(1)                                                                     /*NO.1Ѱ�߲��ж��Ƿ�ʮ��·��*/
		{
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
					TIM2->CR1 |= TIM_CR1_CEN;
				}
        if(Task_Delay[1]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					
				  seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
          if((seek&0X81)&&(seek&0X3C)){TIM2->CR1 |= TIM_CR1_CEN;break;}					
					Task_Delay[1]=1;
					
          TIM2->CR1 |= TIM_CR1_CEN;				
				}				

		}
		delay_ms(100);
		ChangePwm(1204,1124,1092,1167);//���ֶ���200��
    GO_Back();
		delay_ms(70);
		L_Circle();
		delay_ms(300);

		BigLED=1;
		TIM2->CR1 &=~TIM_CR1_CEN;
		while(1)
		{
			seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
			if((seek&0x18)==0x18)break;
		}
		Car_Stop();
		delay_ms(100);

		TIM3->CNT = 400;//������������ƭ
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;

		while(1)//Ѱ�߽���
		{	
					ADJ();
				seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
    TIM2->CR1 |= TIM_CR1_CEN;
		while(1)                                                            /*Ѱ�� ���ж��Ƿ�Ҫ�ϵ�һ�����ΰ�*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
				if(RightCollide==1||LeftCollide==1)break;
		}
		while(1)                                                                       /*�����ΰ���*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
          TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					PID(500,500,500,500);
					
					Task_Delay[0]=10; 
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
				if(RTouch==0||LTouch==0)break;
		}	
		GO_Back();
		delay_ms(50);
		while(1)                                                                       /*�����ΰ��� �ٶȵú���*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
          TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					PID(10,10,10,10);
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
        if(LeftCollide==1||RightCollide==1)break;
		}
	ChangePwm(2858,2553,3205,3323);//���ֶ���200��
		while(1)                                                                     /*NO.2Ѱ�߲��ж��Ƿ�ʮ��·��*/
		{
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
          TIM2->CR1 |= TIM_CR1_CEN;					
				}		
        if(Task_Delay[1]==0)
        {
         TIM2->CR1 &=~TIM_CR1_CEN;					
				 seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
         if((seek&0X81)&&(seek&0X3C)){TIM2->CR1 |= TIM_CR1_CEN;break;}
				 TIM2->CR1 |= TIM_CR1_CEN;///////////////////////////////////////////////////////////////////////////////
			  }
		}
		delay_ms(100);
    GO_Back();
		delay_ms(50);
		ChangePwm(1937,1863,2053,2176);//���ֶ���200��
		R_Circle();
		delay_ms(300);
    
		TIM2->CR1 &=~TIM_CR1_CEN;
		BigLED=0;
		while(1)
		{
			seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
			if((seek&0x18)==0x18)break;
		}
		TIM3->CNT = 400;//������������ƭ
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
    TIM2->CR1 |= TIM_CR1_CEN;
		while(1)//Ѱ�߽���
		{	
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
				if(Task_Delay[1]==0)
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
				  seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
				  if(seek&0x18){TIM2->CR1 |= TIM_CR1_CEN;break;}
				  TIM2->CR1 |= TIM_CR1_CEN;
			  }
		}


		while(1)                                                                        /*Ѱ�߲��ж��Ƿ�Ҫ�ϵ�����ƽ̨*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}			
        if(RightCollide==1||LeftCollide==1)break;	
		}
   ChangePwm(1204,1124,1192,1267);
   delay_ms(650);	//////////////////////////////////////////////////////////12135464651315651			
	 GO_Back();
	 delay_ms(70);
	 Car_Stop();
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 delay_ms(500);
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 ChangePwm(2137,2063,2153,2276);
	 R_Circle();
	 delay_ms(500);
	 delay_ms(300);
	 TIM2->CR1 &=~TIM_CR1_CEN;
	 while(1)
	 {
	  if(Laser==1)break;
	 
	 }
	 TIM2->CR1 |= TIM_CR1_CEN;

		TIM3->CNT = 100;//������������ƭ
		TIM5->CNT = 100;
		TIM4->CNT = 100;
		TIM8->CNT = 100;
    GO_Forward();
	 BIBI=1;
		while(1)//�ջ������ٶ�һ��ʱ��,ʹ�ٶ��ȶ�
		{
		    if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					LED0=~LED0;
				  GO_Forward();
  				PID(100,100,135,135);        
					
					if(LeftCollide==1||RightCollide==1){TIM2->CR1 |= TIM_CR1_CEN;break;}
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}

		}

		BIBI=0;
		while(1)                                                                     /*Ѱ�߲��ж��Ƿ�ʮ��·��*/
		{
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN; 					
				}			
				if(Task_Delay[1]==0)
				{	
					TIM2->CR1 &=~TIM_CR1_CEN;
				  seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
          if((seek&0X81)&&(seek&0X3C))break;
					TIM2->CR1 |= TIM_CR1_CEN;
					Task_Delay[1]=1;
				}	
		}
		delay_ms(200);
		TIM2->CR1 &=~TIM_CR1_CEN;
    GO_Back();
		delay_ms(100);
		ChangePwm(1937,1863,2053,2176);//���ֶ���200��
		L_Circle();
		delay_ms(300);
    
		while(1)
		{
			seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM3->CNT = 400;//������������ƭ
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		
		while(1)                                                                     /*Ѱ��һ��ʱ����ΪҪ�������ĸ�ƽ̨��*/
		{
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
//					i++;
					ADJ();
//					seek=~Recive_Seek();
					Task_Delay[0]=10;
//					if(i>=15){i=0;TIM2->CR1 |= TIM_CR1_CEN;break;}
          TIM2->CR1 |= TIM_CR1_CEN; 					
				}	
        if(Collide==1)break;				
         	
		}		
//		while(1)                                                                       /*����ǰ������ƽ̨*/
//		{	
//        if(Task_Delay[0]==0)//����С����
//				{
//          TIM2->CR1 &=~TIM_CR1_CEN;
//					GO_Forward();
//					PID(100,100,100,100);
//					
//					Task_Delay[0]=10;
//          TIM2->CR1 |= TIM_CR1_CEN;					
//				}
//				if(Collide==1)break;
//		}	//////////////////////////////////////////////////
		
	  ChangePwm(1937,1863,1953,2076);

		TIM2->CR1 &=~TIM_CR1_CEN;
    GO_Back();
		delay_ms(100);
		L_Circle();
		delay_ms(500);
    
		while(1)
		{
			seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;

		TIM3->CNT = 400;//������������ƭ
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		
		while(1)                                                                     /*Ѱ�߲��ж��Ƿ�ʮ��·��*/
		{
        if(Task_Delay[0]==0)//����С����
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN; 					
				}			
				if(Task_Delay[1]==0)
				{	
					TIM2->CR1 &=~TIM_CR1_CEN;
				  seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
          if((seek&0X81)&&(seek&0X3C))break;
					TIM2->CR1 |= TIM_CR1_CEN;
					Task_Delay[1]=1;
				}	
		}
		delay_ms(200);
		TIM2->CR1 &=~TIM_CR1_CEN;
    GO_Back();
		delay_ms(100);
		ChangePwm(1937,1863,2053,2176);//���ֶ���200��
		L_Circle();
		delay_ms(300);
    
		while(1)
		{
			seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek ����Ϊ1 ����Ϊ0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM3->CNT = 400;//������������ƭ
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		while(1)                                                            /*Ѱ�� ���ж��Ƿ�Ҫ�ϵ����ƽ̨*/
		{	
        if(Task_Delay[0]==0)//����С����
				{
          TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
        if(RightCollide==1||LeftCollide==1)break;
		}/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 TIM2->CR1 &=~TIM_CR1_CEN;
   ChangePwm(1204,1124,1192,1267);
   delay_ms(700);	//////////////////////////////////////////////////////////12135464651315651			
	 GO_Back();
	 delay_ms(70);
	 Car_Stop();
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 delay_ms(500);
	 BIBI=1;
	 delay_ms(500);
	 BIBI=0;
	 ChangePwm(2137,2063,2153,2276);
	 R_Circle();
	 delay_ms(500);
	 delay_ms(300);
	 
	 while(1)
	 {
	  if(Laser==1)break;                                               /*��ʼ�µ����ƽ̨��*/
	 
	 }
	 while(1)
		 
   Car_Stop();	 
	 TIM2->CR1 |= TIM_CR1_CEN;
	}
}
 
 
 
