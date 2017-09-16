#include "PWM.h"

//TIM1 PA8 PA9 PA10 PA11
void PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef          GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//ʹ��TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOA
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	TIM_TimeBaseStructure.TIM_Period=arr;//����������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ��
	TIM_TimeBaseStructure.TIM_Prescaler=psc;//������������Ϊ TIMx ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ��
	TIM_TimeBaseStructure.TIM_ClockDivision=0;//������ʱ�ӷָ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//ѡ���˼�����ģʽ��
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//ѡ��ʱ��ģʽ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ�� 
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�������:TIM����Ƚϼ��Ը�
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);//TIM1ͨ��1
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);//TIM1ͨ��2 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);//TIM1ͨ��3
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);//TIM1ͨ��4
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE); 
	
}

