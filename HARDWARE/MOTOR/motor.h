#ifndef __MOTOR_H
#define __MOTOR_H		
#include "sys.h"

#define LeftAhead2  PAout(13)// PA13
#define LeftAhead1  PAout(14)// PA14	

#define LeftBack2   PAout(15)// PA15
#define LeftBack1   PCout(10)// PC10

#define RightAhead1 PCout(11)// PC11
#define RightAhead2 PCout(12)// PC12

#define RightBack1  PGout(11)// PG11
#define RightBack2  PGout(12)// PG12

void Motor_Init(void);
void GO_Forward(void);
void GO_Back(void);
void R_Circle(void);
void L_Circle(void);
void Car_Stop(void);
void GO_Brake(void);
void car_leftStop(void);
void car_rightStop(void);

#endif


