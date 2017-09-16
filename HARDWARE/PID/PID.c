
#include"pid.h"

int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

//int Position_PI (int Encoder,int Target)
//{ 	
//   float Kp=0.02,Ki=0.0002;	
//	 static int Bias,Pwm;
//	 static long Integral_bias;
//	 Bias=Encoder-Target;                //计算偏差
//	 Integral_bias+=Bias;	               //求出偏差的积分
//	 if(Integral_bias>1500000)  Integral_bias=1500000;   //积分限幅
//	 if(Integral_bias<-1500000)  Integral_bias=-1500000; //积分限幅 
//	 Pwm=Kp*Bias+Ki*Integral_bias;       //位置式PI控制器
//	 return Pwm;                         //增量输出

//}


int Incremental_PI (float Kp,float Ki,int Encoder,int Target)
{ 	
   	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Kp*(Bias-Last_bias)+Ki*Bias;   //增量式PI控制器
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}

//void Xianfu_Pwm(int pwm1,int pwm2,int pwm3,int pwm4)
//{	
//	  int Amplitude=7100;    //===PWM满幅是7200 限制在7100
//    if(pwm1<-Amplitude) pwm1=-Amplitude;	
//		if(pwm1>Amplitude)  pwm1=Amplitude;	
//	  if(pwm2<-Amplitude) pwm1=-Amplitude;	
//		if(pwm2>Amplitude)  pwm1=Amplitude;
//	  if(pwm3<-Amplitude) pwm1=-Amplitude;	
//		if(pwm3>Amplitude)  pwm1=Amplitude;
//	  if(pwm4<-Amplitude) pwm1=-Amplitude;	
//		if(pwm4>Amplitude)  pwm1=Amplitude;
//}






