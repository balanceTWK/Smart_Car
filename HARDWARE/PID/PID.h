#ifndef _PID_H_
#define _PID_H_

#include"sys.h"

int Position_PI (int Encoder,int Target);
int Incremental_PI (float Kp,float Ki,int Encoder,int Target);
int myabs(int a);
void Xianfu_Pwm(int pwm1,int pwm2,int pwm3,int pwm4);

#endif


