#include "HMC5883.h"
#include "delay.h"


short int Hmc5883Buff[3];
void Hmc5883_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13); 	//PB6,PB7 �����
}

void Hmc5883_IICAck() //ADXL345 IICӦ��
{
	HmcIIC_SCL=0;
	HmcSDA_OUT();
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SCL=0;
	
}
/*��������������ʱ�����ܵ����һ�������ֽں󣬱�����ӻ�����һ���������͵��źš�
  ����ź��ǶԴӻ��ġ���Ӧ����ʵ�ֵġ�Ȼ�󣬴ӻ��Ż��ͷ�SDA�ߣ�������������STOP�źš�*/
void Hmc5883_IICNAck()//ADXL345 IIC��Ӧ�� 
{
	HmcIIC_SCL=0;
	HmcSDA_OUT();
	HmcIIC_SDA=1;
	delay_us(5);
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SCL=0;	
}

u8 Hmc5883_IICRevAck()//ADXL345 IIC����Ӧ��
{
	u8 x;
	HmcIIC_SCL=1;
	delay_us(5);
	HmcSDA_IN();
	x=HmcIIC_SDA;
	HmcIIC_SCL=0;
	delay_us(5);
	return  x;
}

void Hmc5883_IICStart()//ADXL345 IIC��ʼ�ź�
{
	HmcSDA_OUT();
	HmcIIC_SDA=1;
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=0;
}

void Hmc5883_IICStop() //ADXL345 IICֹͣ�ź�
{
	HmcSDA_OUT();
	HmcIIC_SCL=0;
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=1;
	HmcIIC_SDA=1;
	delay_us(5);
}
void Hmc5883_IIC_SendByte(u8 txd) //ADXL345 IIC ����һ���ֽ�
{
	u8 t;
	HmcSDA_OUT();
	HmcIIC_SCL=0;
	for(t=0;t<8;t++)
	{
        HmcIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(5);  
		HmcIIC_SCL=1;
		delay_us(5); 
		HmcIIC_SCL=0;	
		delay_us(5);		
	}
	Hmc5883_IICRevAck();
}

u8 Hmc5883_IIC_ReadByte()//ADXL345 IIC ��ȡһ���ֽ�
{
	u8 i,receive=0;
	HmcSDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        HmcIIC_SCL=0; 
        delay_us(5);
		    HmcIIC_SCL=1;
        receive<<=1;
        if(HmcREAD_SDA)receive|=0x01;   
		delay_us(5); 
   }					 
    return receive;	
}

//���ֽ�д��
void Single_write_Hmc5883(u8 reg_address,u8 data)
{
	Hmc5883_IICStart();                   //������ʼ�ź�
	Hmc5883_IIC_SendByte(HmcSlaveAddress);   //�����豸��ַ+д�ź�
	Hmc5883_IIC_SendByte(reg_address);        //�ڲ��Ĵ�����ַ
	Hmc5883_IIC_SendByte(data);           //�ڲ��Ĵ���д������
	Hmc5883_IICStop();                    //����ֹͣ�ź�
}

//���ֽڶ�ȡ
u8 Single_Read_Hmc5883(u8 reg_address)
{
	u8 data;
	Hmc5883_IICStart();//��ʼ�ź�
	Hmc5883_IIC_SendByte(HmcSlaveAddress);   //�����豸��ַ+д�ź�
	Hmc5883_IIC_SendByte(reg_address);        //�ڲ��Ĵ�����ַ	
	Hmc5883_IICStart();
	Hmc5883_IIC_SendByte(HmcSlaveAddress+1); //�����豸��ַ+���ź�
	data=Hmc5883_IIC_ReadByte();//��ȡ����
	Hmc5883_IICNAck();//��Ӧ��
	Hmc5883_IICStop();//ֹͣ�ź�
	return data;
}

//��ȡADXL345�ڲ����ٶ�����,��ַ��Χ0x32~0x37
/*ÿ���ֽڱ�����8bit���ȡ����ݴ���ʱ���ȴ����λMSB,
  ÿһ���������ֽں�������һ��Ӧ��λ����һ֡��9��bit����*/
void Multiple_read_Hmc5883()
{
	u8 i;
	u8 BUF[8];
	Hmc5883_IICStart();//��ʼ�ź�
	Hmc5883_IIC_SendByte(HmcSlaveAddress);//�����豸��ַ+д�ź�
	Hmc5883_IIC_SendByte(0x03);//���ʹ洢��Ԫ��ַ ��0x03��ʼ
	Hmc5883_IICStart();//��ʼ�ź�
	Hmc5883_IIC_SendByte(HmcSlaveAddress+1);//���ź�
	for(i=0;i<6;i++)//������ȡ6����ַ�����ݴ���BUF[]
	{
		BUF[i]=Hmc5883_IIC_ReadByte();
		if(i==5) Hmc5883_IICNAck();	//��ȡ���һ�����ݺ� ��Ҫ��NACK
		else Hmc5883_IICAck(); //ÿ��ȡһ�� ����һ����ӦACK
	}
	Hmc5883_IICStop();//ֹͣ�ź�
	Hmc5883Buff[0]=(BUF[1]<<8)+BUF[0]; 
	Hmc5883Buff[1]=(BUF[3]<<8)+BUF[2]; 
	Hmc5883Buff[2]=(BUF[5]<<8)+BUF[4]; 
	delay_ms(5);
}

//��ʼ��Hmc5883
void Init_Hmc5883()
{
	Hmc5883_IIC_Init();
	Single_write_Hmc5883(0x01,0x00);//������Χ����16g,13λģʽ	
	Single_write_Hmc5883(0x02,0x00);//������Χ����16g,13λģʽ	
}



































