#include "seek.h"

void SEEK_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOF,ENABLE);//ʹ��PORTC,PORTFʱ��

	GPIO_InitStructure.GPIO_Pin  = 0x00FF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���ó���������
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOF
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOC

}

u8 Recive_Seek(void)
{
	return GPIO_ReadInputData(GPIOF);
}

u8 Recive_BackSeek(void)
{
	return GPIO_ReadInputData(GPIOD);
}





