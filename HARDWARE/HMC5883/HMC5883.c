#include "HMC5883.h"
#include "delay.h"


short int Hmc5883Buff[3];
void Hmc5883_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13); 	//PB6,PB7 输出高
}

void Hmc5883_IICAck() //ADXL345 IIC应答
{
	HmcIIC_SCL=0;
	HmcSDA_OUT();
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SCL=0;
	
}
/*当主机接收数据时，它受到最后一个数据字节后，必须向从机发出一个结束传送的信号。
  这个信号是对从机的“非应答”来实现的。然后，从机才会释放SDA线，允许主机产生STOP信号。*/
void Hmc5883_IICNAck()//ADXL345 IIC不应答 
{
	HmcIIC_SCL=0;
	HmcSDA_OUT();
	HmcIIC_SDA=1;
	delay_us(5);
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SCL=0;	
}

u8 Hmc5883_IICRevAck()//ADXL345 IIC接收应答
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

void Hmc5883_IICStart()//ADXL345 IIC开始信号
{
	HmcSDA_OUT();
	HmcIIC_SDA=1;
	HmcIIC_SCL=1;
	delay_us(5);
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=0;
}

void Hmc5883_IICStop() //ADXL345 IIC停止信号
{
	HmcSDA_OUT();
	HmcIIC_SCL=0;
	HmcIIC_SDA=0;
	delay_us(5);
	HmcIIC_SCL=1;
	HmcIIC_SDA=1;
	delay_us(5);
}
void Hmc5883_IIC_SendByte(u8 txd) //ADXL345 IIC 发送一个字节
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

u8 Hmc5883_IIC_ReadByte()//ADXL345 IIC 读取一个字节
{
	u8 i,receive=0;
	HmcSDA_IN();//SDA设置为输入
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

//单字节写入
void Single_write_Hmc5883(u8 reg_address,u8 data)
{
	Hmc5883_IICStart();                   //发送起始信号
	Hmc5883_IIC_SendByte(HmcSlaveAddress);   //发送设备地址+写信号
	Hmc5883_IIC_SendByte(reg_address);        //内部寄存器地址
	Hmc5883_IIC_SendByte(data);           //内部寄存器写入数据
	Hmc5883_IICStop();                    //发送停止信号
}

//单字节读取
u8 Single_Read_Hmc5883(u8 reg_address)
{
	u8 data;
	Hmc5883_IICStart();//开始信号
	Hmc5883_IIC_SendByte(HmcSlaveAddress);   //发送设备地址+写信号
	Hmc5883_IIC_SendByte(reg_address);        //内部寄存器地址	
	Hmc5883_IICStart();
	Hmc5883_IIC_SendByte(HmcSlaveAddress+1); //发送设备地址+读信号
	data=Hmc5883_IIC_ReadByte();//读取数据
	Hmc5883_IICNAck();//无应答
	Hmc5883_IICStop();//停止信号
	return data;
}

//读取ADXL345内部加速度数据,地址范围0x32~0x37
/*每个字节必须是8bit长度。数据传送时，先传最高位MSB,
  每一个被传送字节后面必须跟一个应答位（即一帧共9个bit）。*/
void Multiple_read_Hmc5883()
{
	u8 i;
	u8 BUF[8];
	Hmc5883_IICStart();//开始信号
	Hmc5883_IIC_SendByte(HmcSlaveAddress);//发送设备地址+写信号
	Hmc5883_IIC_SendByte(0x03);//发送存储单元地址 从0x03开始
	Hmc5883_IICStart();//开始信号
	Hmc5883_IIC_SendByte(HmcSlaveAddress+1);//读信号
	for(i=0;i<6;i++)//连续读取6个地址的数据存入BUF[]
	{
		BUF[i]=Hmc5883_IIC_ReadByte();
		if(i==5) Hmc5883_IICNAck();	//读取最后一个数据后 需要回NACK
		else Hmc5883_IICAck(); //每读取一组 产生一个回应ACK
	}
	Hmc5883_IICStop();//停止信号
	Hmc5883Buff[0]=(BUF[1]<<8)+BUF[0]; 
	Hmc5883Buff[1]=(BUF[3]<<8)+BUF[2]; 
	Hmc5883Buff[2]=(BUF[5]<<8)+BUF[4]; 
	delay_ms(5);
}

//初始化Hmc5883
void Init_Hmc5883()
{
	Hmc5883_IIC_Init();
	Single_write_Hmc5883(0x01,0x00);//测量范围正负16g,13位模式	
	Single_write_Hmc5883(0x02,0x00);//测量范围正负16g,13位模式	
}



































