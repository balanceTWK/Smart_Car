#ifndef __HMC5883_H
#define __HMC5883_H
#include "sys.h"

#define	HmcSlaveAddress   0x3C	

//IO方向设置
#define HmcSDA_IN()  {GPIOB->CRH&=0XFFF0FFFF;GPIOB->CRH|=8<<16;}
#define HmcSDA_OUT() {GPIOB->CRH&=0XFFF0FFFF;GPIOB->CRH|=3<<16;}

#define HmcIIC_SCL    PBout(13) //SCL
#define HmcIIC_SDA    PBout(12) //SDA	 
#define HmcREAD_SDA   PBin(12)  //输入SDA 

void Hmc5883_IIC_Init(void);
void Hmc5883_IICAck(void);
void Hmc5883_IICNAck(void);
u8 Hmc5883_IICRevAck(void);
void Hmc5883_IICStart(void);
void Hmc5883_IICStop(void);
void Hmc5883_IIC_SendByte(u8 txd);
u8 Hmc5883_IIC_ReadByte(void);
void Single_write_Hmc5883(u8 reg_address,u8 data);
u8 Single_Read_Hmc5883(u8 reg_address);
void Multiple_read_Hmc5883(void);
void Init_Hmc5883(void);


#endif


