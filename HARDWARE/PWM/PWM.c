#include "PWM.h"

//TIM1 PA8 PA9 PA10 PA11
void PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef          GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//使能TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	TIM_TimeBaseStructure.TIM_Period=arr;//设置了在下一个更新事件装入活动的自动重装载寄存器周期的值。
	TIM_TimeBaseStructure.TIM_Prescaler=psc;//设置了用来作为 TIMx 时钟频率除数的预分频值。
	TIM_TimeBaseStructure.TIM_ClockDivision=0;//设置了时钟分割。
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//选择了计数器模式。
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//选择定时器模式。
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能 
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性:TIM输出比较极性高
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);//TIM1通道1
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);//TIM1通道2 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);//TIM1通道3
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);//TIM1通道4
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE); 
	
}

