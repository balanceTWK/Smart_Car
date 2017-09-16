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
 STM32F103ZET6工程
 作者：唐伟康
************************************************/

int i;//有多处调用，注意每次使用后要清零,免下次调用出错.

extern short int ADXL345Buff[3];//三轴加速度计数据xyz；
extern u16 Task_Delay[NumOfTask];//NumOfTask表示每个阶段下的并行小任务个数.
extern u8 state;//1为前进,0为倒退,3为顺时针  在motor.c
u16 v1,v2,v3,v4;//小车期望速度
u16 speed1,speed2,speed3,speed4;//小车实时速度
int  pwm1=0,pwm2=0,pwm3=0,pwm4=0; //小车电机PWM值
u8 key;//按键扫描
volatile u8 backseek,seek;//循迹数据

u8 signal=0;//判断小车平衡状态，第一位表示加减速度,第二位表示上坡,第三位表示下坡；该变量在 TIMER.C中引用；
void GetSpeed()
{
		speed1=(TIM3->CNT);
		speed2=(TIM4->CNT);
		speed3=(TIM5->CNT);
		speed4=(TIM8->CNT);
	 if(state==1)//判断为前进
	 {
	 	speed3=65536-speed3;
		speed4=65536-speed4;
	 }
	 else if(state==2)//判断为后退
	 {
	 	speed1=65536-speed1;
		speed2=65536-speed2;
	 }
	 else if(state==3)//判断为顺时针转圈
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
		TIM3->CNT = 0;//编码器计数清零
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
	 if(state==1)//判断为前进
	 {
	 	speed3=65536-speed3;
		speed4=65536-speed4;
	 }
	 else if(state==2)//判断为后退
	 {
	 	speed1=65536-speed1;
		speed2=65536-speed2;
	 }
	 else if(state==3)//判断为顺时针转圈
	 {
    LED0=~LED0;
	 }	 
	 count=speed1+speed2+speed3+speed4;
	 return count;
}


void PID(int sv1,int sv2,int sv3,int sv4)//连续调用此函数 每次调用必须得有时间间隔，时间间隔，50ms为最佳。
{
//	int error;
	  GetSpeed();
		sv1+=3;//误差校正
		sv2+=3;//误差校正
	
//	error=(speed1+speed2+speed3+speed4-sv1-sv2-sv3-sv4);
//	if((error>200)||(error<-200))signal|=0x01;
//	else signal&=0xFE;
	
		if(speed1>sv1){pwm1=(int16_t)(pwm1+4*(speed1-sv1));}      //闭环控制
		else if(sv1>speed1){(int16_t)(pwm1=pwm1-4*(sv1-speed1));} //闭环控制
		if(pwm1>=6000)pwm1=6000;                          
		if(pwm1<=0)pwm1=0;                                  
		
		if(speed2>sv2){pwm2=(int16_t)(pwm2+4*(speed2-sv2));}      //闭环控制
		else if(sv2>speed2){(int16_t)(pwm2=pwm2-4*(sv2-speed2));} //闭环控制
		if(pwm2>=6000)pwm2=6000;                             
		if(pwm2<=0)pwm2=0;                                  
		
		if(speed3>sv3){pwm3=(int16_t)(pwm3+4*(speed3-sv3));}      //闭环控制
		else if(sv3>speed3){(int16_t)(pwm3=pwm3-4*(sv3-speed3));} //闭环控制
		if(pwm3>=6000)pwm3=6000;                            
		if(pwm3<=0)pwm3=0;                                  
		
		if(speed4>sv4){pwm4=(int16_t)(pwm4+4*(speed4-sv4));}      //闭环控制
		else if(sv4>speed4){(int16_t)(pwm4=pwm4-4*(sv4-speed4));} //闭环控制
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
	seek=~Recive_Seek();//取反后seek 白线为1 黑线为0;
	
			  if((seek&0x18)==0x18){GO_Forward();PID(400,400,400,400);}//0001 1000  传感器编码是反装的的,故判断也是反的 
			  else if (seek&0x08){GO_Forward();ChangePwm(2268,2183,1560,1632);}//0001 0000  传感器编码是反装的的,故判断也是反的
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
	seek=~Recive_Seek();//取反后seek 白线为1 黑线为0;
	
////			  if((seek&0x18)==0x18){GO_Forward();PID(400,400,400,400);}//0001 1000  传感器编码是反装的的,故判断也是反的 
//			  else 
if (seek&0x08){car_leftStop();}//0001 0000  传感器编码是反装的的,故判断也是反的
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
	seek=~Recive_Seek();//取反后seek 白线为1 黑线为0;
	
			  if((seek&0x18)==0x18)PID(100,100,100,100);//0001 1000  传感器编码是反装的的,故判断也是反的
			  else if (seek&0x08)ChangePwm(3258,2933,2933,2495);//0001 0000  传感器编码是反装的的,故判断也是反的
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	BigLED_Init();//蜂鸣器初始化
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	I2C_Configuration();
	OLED_Init();	
	Init_ADXL345();//三轴加速度计初始化
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	PWM_Init(7199,0);//PWM 不分频。PWM频率=72000/(7199+1)=10Khz。
	SPEED_Init(0xFFFF,0);		//以72Mhz的频率计数 0，0
	Motor_Init();///////////////////////////////////////////////////////////////////////////////////
	Car_Stop();
	KEY_Init();
	SEEK_Init();
	Init_Hmc5883();//地磁计
	OLED_Fill(0xFF);//全屏点亮
	delay_ms(200);
	OLED_Fill(0x00);//全屏灭	
	
//	TIM6_Int_Init(2999,7199);//====30ms进一次中断服务函数,中断服务函数在TIMER.c   溢出时间（单位为 us）= ((arr+1)*(psc+1))/72；
	TIM2_Int_Init(49,7199);//=====10MS进一次中断服务函数，中断服务函数在TIMER.c
	delay_ms(20);
	
  while(1)
	{
		
		while(1)                                                             /*开始就绪,等待触发*/
		{
			Car_Stop();
			if(Bhend==0)
			{
			delay_ms(20);
       if(Bhend==0)break;
			}
	     key=KEY_Scan(0);
		   if(key==KEY0_PRES)//并行空闲小任务
			  {
			    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
					while(1){LED0=~LED0; LED1=~LED0;delay_ms(1000);}
			  }			
		}
		BIBI=1;
		while(1)                                                                     /*闭环控制速度一段时间,使速度稳定.*/
		{
		    if(Task_Delay[0]==0)//并行小任务
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
        if(Task_Delay[0]==0)//并行小任务
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
		while(1)                                                                        /*寻线并判断是否要上第二个平台*/
		{	
        if(Task_Delay[0]==0)//并行小任务
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

		TIM3->CNT = 100;//编码器计数欺骗
		TIM5->CNT = 100;
		TIM4->CNT = 100;
		TIM8->CNT = 100;
    GO_Forward();
		BIBI=1;
		while(1)                                                                     /*闭环控制速度一段时间,使速度稳定.*/
		{
		    if(Task_Delay[0]==0)//并行小任务
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
		while(1)                                                                     /*NO.1寻线并判断是否到十字路口*/
		{
        if(Task_Delay[0]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
					TIM2->CR1 |= TIM_CR1_CEN;
				}
        if(Task_Delay[1]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					
				  seek=~Recive_Seek();//seek 白线为1 黑线为0;
          if((seek&0X81)&&(seek&0X3C)){TIM2->CR1 |= TIM_CR1_CEN;break;}					
					Task_Delay[1]=1;
					
          TIM2->CR1 |= TIM_CR1_CEN;				
				}				

		}
		delay_ms(100);
		ChangePwm(1204,1124,1092,1167);//左轮都减200了
    GO_Back();
		delay_ms(70);
		L_Circle();
		delay_ms(300);

		BigLED=1;
		TIM2->CR1 &=~TIM_CR1_CEN;
		while(1)
		{
			seek=~Recive_Seek();//seek 白线为1 黑线为0;
			if((seek&0x18)==0x18)break;
		}
		Car_Stop();
		delay_ms(100);

		TIM3->CNT = 400;//编码器计数欺骗
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;

		while(1)//寻线矫正
		{	
					ADJ();
				seek=~Recive_Seek();//seek 白线为1 黑线为0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
    TIM2->CR1 |= TIM_CR1_CEN;
		while(1)                                                            /*寻线 并判断是否要上第一个跷跷板*/
		{	
        if(Task_Delay[0]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
				if(RightCollide==1||LeftCollide==1)break;
		}
		while(1)                                                                       /*上跷跷板中*/
		{	
        if(Task_Delay[0]==0)//并行小任务
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
		while(1)                                                                       /*下跷跷板中 速度得很慢*/
		{	
        if(Task_Delay[0]==0)//并行小任务
				{
          TIM2->CR1 &=~TIM_CR1_CEN;
					GO_Forward();
					PID(10,10,10,10);
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN;					
				}
        if(LeftCollide==1||RightCollide==1)break;
		}
	ChangePwm(2858,2553,3205,3323);//左轮都减200了
		while(1)                                                                     /*NO.2寻线并判断是否到十字路口*/
		{
        if(Task_Delay[0]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10; 
          TIM2->CR1 |= TIM_CR1_CEN;					
				}		
        if(Task_Delay[1]==0)
        {
         TIM2->CR1 &=~TIM_CR1_CEN;					
				 seek=~Recive_Seek();//seek 白线为1 黑线为0;
         if((seek&0X81)&&(seek&0X3C)){TIM2->CR1 |= TIM_CR1_CEN;break;}
				 TIM2->CR1 |= TIM_CR1_CEN;///////////////////////////////////////////////////////////////////////////////
			  }
		}
		delay_ms(100);
    GO_Back();
		delay_ms(50);
		ChangePwm(1937,1863,2053,2176);//左轮都减200了
		R_Circle();
		delay_ms(300);
    
		TIM2->CR1 &=~TIM_CR1_CEN;
		BigLED=0;
		while(1)
		{
			seek=~Recive_Seek();//seek 白线为1 黑线为0;
			if((seek&0x18)==0x18)break;
		}
		TIM3->CNT = 400;//编码器计数欺骗
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
    TIM2->CR1 |= TIM_CR1_CEN;
		while(1)//寻线矫正
		{	
        if(Task_Delay[0]==0)//并行小任务
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
				  seek=~Recive_Seek();//seek 白线为1 黑线为0;
				  if(seek&0x18){TIM2->CR1 |= TIM_CR1_CEN;break;}
				  TIM2->CR1 |= TIM_CR1_CEN;
			  }
		}


		while(1)                                                                        /*寻线并判断是否要上第三个平台*/
		{	
        if(Task_Delay[0]==0)//并行小任务
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

		TIM3->CNT = 100;//编码器计数欺骗
		TIM5->CNT = 100;
		TIM4->CNT = 100;
		TIM8->CNT = 100;
    GO_Forward();
	 BIBI=1;
		while(1)//闭环控制速度一段时间,使速度稳定
		{
		    if(Task_Delay[0]==0)//并行小任务
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
		while(1)                                                                     /*寻线并判断是否到十字路口*/
		{
        if(Task_Delay[0]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN; 					
				}			
				if(Task_Delay[1]==0)
				{	
					TIM2->CR1 &=~TIM_CR1_CEN;
				  seek=~Recive_Seek();//seek 白线为1 黑线为0;
          if((seek&0X81)&&(seek&0X3C))break;
					TIM2->CR1 |= TIM_CR1_CEN;
					Task_Delay[1]=1;
				}	
		}
		delay_ms(200);
		TIM2->CR1 &=~TIM_CR1_CEN;
    GO_Back();
		delay_ms(100);
		ChangePwm(1937,1863,2053,2176);//左轮都减200了
		L_Circle();
		delay_ms(300);
    
		while(1)
		{
			seek=~Recive_Seek();//seek 白线为1 黑线为0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek 白线为1 黑线为0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM3->CNT = 400;//编码器计数欺骗
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		
		while(1)                                                                     /*寻线一段时间因为要触碰第四个平台了*/
		{
        if(Task_Delay[0]==0)//并行小任务
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
//		while(1)                                                                       /*缓速前进触碰平台*/
//		{	
//        if(Task_Delay[0]==0)//并行小任务
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
			seek=~Recive_Seek();//seek 白线为1 黑线为0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek 白线为1 黑线为0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;

		TIM3->CNT = 400;//编码器计数欺骗
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		
		while(1)                                                                     /*寻线并判断是否到十字路口*/
		{
        if(Task_Delay[0]==0)//并行小任务
				{
					TIM2->CR1 &=~TIM_CR1_CEN;
					SeekADJ();
					
					Task_Delay[0]=10;
          TIM2->CR1 |= TIM_CR1_CEN; 					
				}			
				if(Task_Delay[1]==0)
				{	
					TIM2->CR1 &=~TIM_CR1_CEN;
				  seek=~Recive_Seek();//seek 白线为1 黑线为0;
          if((seek&0X81)&&(seek&0X3C))break;
					TIM2->CR1 |= TIM_CR1_CEN;
					Task_Delay[1]=1;
				}	
		}
		delay_ms(200);
		TIM2->CR1 &=~TIM_CR1_CEN;
    GO_Back();
		delay_ms(100);
		ChangePwm(1937,1863,2053,2176);//左轮都减200了
		L_Circle();
		delay_ms(300);
    
		while(1)
		{
			seek=~Recive_Seek();//seek 白线为1 黑线为0;
			if((seek&0x18)==0x18)break;
		}
		
		while(1)                                                            
		{	
					ADJ();
				seek=~Recive_Seek();//seek 白线为1 黑线为0;
				if(seek&0x18){Car_Stop(); delay_ms(50); if(seek&0x18)break;}
		}
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM3->CNT = 400;//编码器计数欺骗
		TIM5->CNT = 400;
		TIM4->CNT = 400;
		TIM8->CNT = 400;
		while(1)                                                            /*寻线 并判断是否要上第五个平台*/
		{	
        if(Task_Delay[0]==0)//并行小任务
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
	  if(Laser==1)break;                                               /*开始下第五个平台了*/
	 
	 }
	 while(1)
		 
   Car_Stop();	 
	 TIM2->CR1 |= TIM_CR1_CEN;
	}
}
 
 
 
