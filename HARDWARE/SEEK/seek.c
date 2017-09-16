#include "seek.h"

void SEEK_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOF,ENABLE);//使能PORTC,PORTF时钟

	GPIO_InitStructure.GPIO_Pin  = 0x00FF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置成上拉输入
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOC

}

u8 Recive_Seek(void)
{
	return GPIO_ReadInputData(GPIOF);
}

u8 Recive_BackSeek(void)
{
	return GPIO_ReadInputData(GPIOD);
}





