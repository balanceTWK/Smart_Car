#ifndef __ADXL345_IIC_H
#define __ADXL345_IIC_H
#include "sys.h"

#define	SlaveAddress   0xA6	

////IO��������
//#define SDA_IN()  {GPIOG->CRH&=0XFFFFFF0F;GPIOC->CRH|=8<<4;}
//#define SDA_OUT() {GPIOG->CRH&=0XFFFFFF0F;GPIOC->CRH|=3<<4;}

//#define IIC_SCL    PGout(10) //SCL
//#define IIC_SDA    PGout(9) //SDA	 
//#define READ_SDA   PGin(9)  //����SDA 

//IO��������
 
#define SDA_IN()  {GPIOE->CRL&=0XFFFFFF0F;GPIOE->CRL|=(u32)8<<4;}
#define SDA_OUT() {GPIOE->CRL&=0XFFFFFF0F;GPIOE->CRL|=(u32)3<<4;}

//IO��������	 
#define IIC_SCL    PEout(0) //SCL
#define IIC_SDA    PEout(1) //SDA	 
#define READ_SDA   PEin(1)  //����SDA 

void IIC_Init(void);//IIC��ʼ��
void ADXL345_IICAck(void);//IICӦ��
void ADXL345_IICNAck(void);//IIC��Ӧ��
u8 ADXL345_IICRevAck(void);//IIC����Ӧ��
void ADXL345_IICStart(void);//IIC��ʼ�ź�
void ADXL345_IICStop(void);//IICֹͣ�ź�
void ADXL345_IIC_SendByte(u8 txd);//����һ���ֽ�
u8 ADXL345_IIC_ReadByte(void);//��ȡһ���ֽ�
void Single_write_ADXL345(u8 reg_address,u8 data);//���ֽ�д��
u8 Single_Read_ADXL345(u8 reg_address);//���ֽڶ�ȡ
void Multiple_read_ADXL345(void);//ADXL345���ݶ�ȡ
void Init_ADXL345(void);//��ʼ��ADXL345



#endif




