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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM6初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM6中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}
//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM6中断
{
	u8 i;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志
//printf("b：%d c：%d pwm1：%d pwm2：%d pwm3：%d pwm4：%d speed1：%d  speed2：%d  speed3：%d  speed4：%d  \r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4,speed1,speed2,speed3,speed4);
//printf("b：%d c：%d pwm1：%d pwm2：%d pwm3：%d pwm4：%d\r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4);
// printf("speed1：%d speed2：%d speed3：%d speed4：%d\r\n",
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
	
	//定时器TIM6初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM6中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM6, ENABLE);  //使能TIMx					 
}
//定时器2中断服务程序
void TIM6_IRQHandler(void)   //TIM6中断
{
//	static u8 count1=0;
//	static u8 count2=0;
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM6更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志
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
			
//printf("b：%d c：%d pwm1：%d pwm2：%d pwm3：%d pwm4：%d speed1：%d  speed2：%d  speed3：%d  speed4：%d  \r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4,speed1,speed2,speed3,speed4);
//printf("b：%d c：%d pwm1：%d pwm2：%d pwm3：%d pwm4：%d\r\n",
//		     b,c,pwm1,pwm2,pwm3,pwm4);
// printf("speed1：%d speed2：%d speed3：%d speed4：%d\r\n",
//		     speed1,speed2,speed3,speed4);
//			printf("signal %d x : %d y : %d  z : %d   angle: %f\r\n",
//		     signal,ADXL345Buff[0],ADXL345Buff[1],ADXL345Buff[2],angle);
//			printf(" angle: %f\r\n",angle);
//			printf(" Z: %d\r\n",Hmc5883Buff[2]);
		}
}



