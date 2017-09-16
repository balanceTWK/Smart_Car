#include "ADXL345_IIC.h"
#include "delay.h"


short int ADXL345Buff[3];
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOE, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_0|GPIO_Pin_1); 	//PB6,PB7 输出高
}
//void IIC_Init()//IIC初始化
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOG, ENABLE );

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOG, &GPIO_InitStructure);
//	
//	IIC_SCL=1;
//	IIC_SDA=1;
//}
void ADXL345_IICAck() //ADXL345 IIC应答
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
	
}
/*当主机接收数据时，它受到最后一个数据字节后，必须向从机发出一个结束传送的信号。
  这个信号是对从机的“非应答”来实现的。然后，从机才会释放SDA线，允许主机产生STOP信号。*/
void ADXL345_IICNAck()//ADXL345 IIC不应答 
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;	
}

u8 ADXL345_IICRevAck()//ADXL345 IIC接收应答
{
	u8 x;
	IIC_SCL=1;
	delay_us(5);
	SDA_IN();
	x=IIC_SDA;
	IIC_SCL=0;
	delay_us(5);
	return  x;
}

void ADXL345_IICStart()//ADXL345 IIC开始信号
{
	SDA_OUT();
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(5);
	IIC_SDA=0;
	delay_us(5);
	IIC_SCL=0;
}

void ADXL345_IICStop() //ADXL345 IIC停止信号
{
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;
	delay_us(5);
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(5);
}
void ADXL345_IIC_SendByte(u8 txd) //ADXL345 IIC 发送一个字节
{
	u8 t;
	SDA_OUT();
	IIC_SCL=0;
	for(t=0;t<8;t++)
	{
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(5);  
		IIC_SCL=1;
		delay_us(5); 
		IIC_SCL=0;	
		delay_us(5);		
	}
	ADXL345_IICRevAck();
}

u8 ADXL345_IIC_ReadByte()//ADXL345 IIC 读取一个字节
{
	u8 i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(5);
		    IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive|=0x01;   
		delay_us(5); 
   }					 
    return receive;	
}

//单字节写入
void Single_write_ADXL345(u8 reg_address,u8 data)
{
	ADXL345_IICStart();                   //发送起始信号
	ADXL345_IIC_SendByte(SlaveAddress);   //发送设备地址+写信号
	ADXL345_IIC_SendByte(reg_address);        //内部寄存器地址
	ADXL345_IIC_SendByte(data);           //内部寄存器写入数据
	ADXL345_IICStop();                    //发送停止信号
}

//单字节读取
u8 Single_Read_ADXL345(u8 reg_address)
{
	u8 data;
	ADXL345_IICStart();//开始信号
	ADXL345_IIC_SendByte(SlaveAddress);   //发送设备地址+写信号
	ADXL345_IIC_SendByte(reg_address);        //内部寄存器地址	
	ADXL345_IICStart();
	ADXL345_IIC_SendByte(SlaveAddress+1); //发送设备地址+读信号
	data=ADXL345_IIC_ReadByte();//读取数据
	ADXL345_IICNAck();//无应答
	ADXL345_IICStop();//停止信号
	return data;
}

//读取ADXL345内部加速度数据,地址范围0x32~0x37
/*每个字节必须是8bit长度。数据传送时，先传最高位MSB,
  每一个被传送字节后面必须跟一个应答位（即一帧共9个bit）。*/
void Multiple_read_ADXL345()
{
	u8 i;
	u8 BUF[8];
	ADXL345_IICStart();//开始信号
	ADXL345_IIC_SendByte(SlaveAddress);//发送设备地址+写信号
	ADXL345_IIC_SendByte(0x32);//发送存储单元地址 从0x32
	ADXL345_IICStart();//开始信号
	ADXL345_IIC_SendByte(SlaveAddress+1);//读信号
	for(i=0;i<6;i++)//连续读取6个地址的数据存入BUF[]
	{
		BUF[i]=ADXL345_IIC_ReadByte();
		if(i==5) ADXL345_IICNAck();	//读取最后一个数据后 需要回NACK
		else ADXL345_IICAck(); //每读取一组 产生一个回应ACK
	}
	ADXL345_IICStop();//停止信号
	ADXL345Buff[0]=(BUF[1]<<8)+BUF[0]; 
	ADXL345Buff[1]=(BUF[3]<<8)+BUF[2]; 
	ADXL345Buff[2]=(BUF[5]<<8)+BUF[4]; 
	delay_ms(5);
}

//初始化ADXL345
void Init_ADXL345()
{
	IIC_Init();
	Single_write_ADXL345(0x31,0x0B);//测量范围正负16g,13位模式
	Single_write_ADXL345(0x2C,0x08);//速率设定为12.5 参考pdf13页
	Single_write_ADXL345(0x2D,0x08);//选择电源模式 参考pdf24页
	Single_write_ADXL345(0x2E,0x80);//使能 data ready 中断
	Single_write_ADXL345(0x1E,0x00);//x偏移量 根据测试传感器写入  pdf29
	Single_write_ADXL345(0x1F,0x00);//y偏移量 根据测试传感器写入  pdf29
	Single_write_ADXL345(0x20,0x05);//z偏移量 根据测试传感器写入  pdf29
	
}































