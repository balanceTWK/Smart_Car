
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
//	 Bias=Encoder-Target;                //����ƫ��
//	 Integral_bias+=Bias;	               //���ƫ��Ļ���
//	 if(Integral_bias>1500000)  Integral_bias=1500000;   //�����޷�
//	 if(Integral_bias<-1500000)  Integral_bias=-1500000; //�����޷� 
//	 Pwm=Kp*Bias+Ki*Integral_bias;       //λ��ʽPI������
//	 return Pwm;                         //�������

//}


int Incremental_PI (float Kp,float Ki,int Encoder,int Target)
{ 	
   	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //����ƫ��
	 Pwm+=Kp*(Bias-Last_bias)+Ki*Bias;   //����ʽPI������
	 Last_bias=Bias;	                   //������һ��ƫ�� 
	 return Pwm;                         //�������
}

//void Xianfu_Pwm(int pwm1,int pwm2,int pwm3,int pwm4)
//{	
//	  int Amplitude=7100;    //===PWM������7200 ������7100
//    if(pwm1<-Amplitude) pwm1=-Amplitude;	
//		if(pwm1>Amplitude)  pwm1=Amplitude;	
//	  if(pwm2<-Amplitude) pwm1=-Amplitude;	
//		if(pwm2>Amplitude)  pwm1=Amplitude;
//	  if(pwm3<-Amplitude) pwm1=-Amplitude;	
//		if(pwm3>Amplitude)  pwm1=Amplitude;
//	  if(pwm4<-Amplitude) pwm1=-Amplitude;	
//		if(pwm4>Amplitude)  pwm1=Amplitude;
//}






