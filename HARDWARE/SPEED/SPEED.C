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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);//使能由APB1控制的TIM334端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC,ENABLE);//使能GPIOA与GPIOB,GPIOC 由APB2控制的TIM8端口时钟
	
	TIM_DeInit(TIM8);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	TIM_DeInit(TIM5);//TIM复位
	
	//TIM8 PC6 PC7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PC6 PC7 浮空输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//TIM3 PA6 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//TIM4 PD12 PD13
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);        //TIM4引脚重映射
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//TIM5 PA0 PA1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//各个TIM2345编码模式
	TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//个编码器均设置为模式3,上升沿有效.
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

	TIM_TimeBaseStructure.TIM_Period = arr;   			//设置计数器溢出后的重载初值
  TIM_TimeBaseStructure.TIM_Prescaler = psc; 	  			//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); 		//用以上参数初始化定时器时间基础模块		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 			
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 			
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 		

  TIM_ICStructInit(&TIM_ICInitStructure);//将该结构体恢复为缺省项
  TIM_ICInitStructure.TIM_ICFilter = 0xF;//滤波
	
  TIM_ICInit(TIM8, &TIM_ICInitStructure);//定时器输入端口初始化
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
	
	TIM_Cmd(TIM8, ENABLE); 	//使能定时器8	
	TIM_Cmd(TIM3, ENABLE); 	//使能定时器3	
	TIM_Cmd(TIM4, ENABLE); 	//使能定时器4	
	TIM_Cmd(TIM5, ENABLE); 	//使能定时器5	
	

}






