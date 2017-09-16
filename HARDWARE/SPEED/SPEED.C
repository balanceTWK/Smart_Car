#include "SPEED.h"

//     CH1   CH2
//TIM8 PC6   PC7
//TIM3 PA6   PA7
//TIM4 PD12  PD13
//TIM5 PA0   PA1

void SPEED_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);//ʹ����APB1���Ƶ�TIM334�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��GPIOA��GPIOB,GPIOC ��APB2���Ƶ�TIM8�˿�ʱ��
	
	TIM_DeInit(TIM8);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	TIM_DeInit(TIM5);//TIM��λ
	
	//TIM8 PC6 PC7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PC6 PC7 ��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//TIM3 PA6 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//TIM4 PD12 PD13
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);        //TIM4������ӳ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//TIM5 PA0 PA1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//����TIM2345����ģʽ
	TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//��������������Ϊģʽ3,��������Ч.
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

	TIM_TimeBaseStructure.TIM_Period = arr;   			//���ü��������������س�ֵ
  TIM_TimeBaseStructure.TIM_Prescaler = psc; 	  			//��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); 		//�����ϲ�����ʼ����ʱ��ʱ�����ģ��		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 			
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 			
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 		

  TIM_ICStructInit(&TIM_ICInitStructure);//���ýṹ��ָ�Ϊȱʡ��
  TIM_ICInitStructure.TIM_ICFilter = 0xF;//�˲�
	
  TIM_ICInit(TIM8, &TIM_ICInitStructure);//��ʱ������˿ڳ�ʼ��
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
	
	TIM_Cmd(TIM8, ENABLE); 	//ʹ�ܶ�ʱ��8	
	TIM_Cmd(TIM3, ENABLE); 	//ʹ�ܶ�ʱ��3	
	TIM_Cmd(TIM4, ENABLE); 	//ʹ�ܶ�ʱ��4	
	TIM_Cmd(TIM5, ENABLE); 	//ʹ�ܶ�ʱ��5	
	

}





