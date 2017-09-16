#include "motor.h"
#include "sys.h"
extern u16 v1,v2,v3,v4;


u8 state;//1Ϊǰ��,0Ϊ����,3Ϊ˳ʱ��


void Motor_Init() //IO��ʼ��
{ 
	
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOG,ENABLE);//ʹ��PORTA,PORTEʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA13,14,15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC10,11,12
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIOG11,12

}



void GO_Forward()
{
	state=1;
	LeftAhead1=1;  
	LeftAhead2=0;	

  LeftBack1=1;   
  LeftBack2=0;   

  RightAhead1=1; 
	RightAhead2=0; 

  RightBack1=1;  
  RightBack2=0;  
}

void GO_Back()
{
	state=2;
	LeftAhead1=0;  
	LeftAhead2=1;	

  LeftBack1=0;   
  LeftBack2=1;   

  RightAhead1=0; 
	RightAhead2=1; 

  RightBack1=0;  
  RightBack2=1;
}

void R_Circle()
{
	state=3;
	LeftAhead1=1;  
	LeftAhead2=0;	

  LeftBack1=1;   
  LeftBack2=0;   

  RightAhead1=0; 
	RightAhead2=1; 

  RightBack1=0;  
  RightBack2=1;

}
void L_Circle()
{
	state=5;
	LeftAhead1=0;  
	LeftAhead2=1;	

  LeftBack1=0;   
  LeftBack2=1;   

  RightAhead1=1; 
	RightAhead2=0; 

  RightBack1=1;  
  RightBack2=0;

}
void Car_Stop()
{
	state=4;
	LeftAhead1=1;  
	LeftAhead2=1;	

  LeftBack1=1;   
  LeftBack2=1;   

  RightAhead1=1; 
	RightAhead2=1; 

  RightBack1=1;  
  RightBack2=1;  
}


void GO_Brake()//�ƶ�
{
	state=6;
	LeftAhead1=0;  
	LeftAhead2=1;	

  LeftBack1=0;   
  LeftBack2=1;   

  RightAhead1=1; 
	RightAhead2=1; 

  RightBack1=1;  
  RightBack2=1;  
}

void car_leftStop()
{
	state=7;
	LeftAhead1=1;  
	LeftAhead2=1;	

  LeftBack1=1;   
  LeftBack2=1;   

  RightAhead1=1; 
	RightAhead2=0; 

  RightBack1=1;  
  RightBack2=0; 

}

void car_rightStop()
{
	state=7;
	LeftAhead1=1;  
	LeftAhead2=0;	

  LeftBack1=1;   
  LeftBack2=0;   

  RightAhead1=1; 
	RightAhead2=1; 

  RightBack1=1;  
  RightBack2=1; 

}










