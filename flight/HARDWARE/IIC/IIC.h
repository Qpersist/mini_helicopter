#ifndef _IIC_H_
#define _IIC_H_
#include "sys.h"

/*函数声明----------------------------------------------------------------*/

#define SCL     GPIO_Pin_6 //24C02 SCL
#define SDA     GPIO_Pin_7 //24C02 SDA 

//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)1<<28;}
//IO操作函数	
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA
#define SDA_H               GPIO_SetBits(GPIOB,SDA)          /*SDA做输出*/
#define SDA_L               GPIO_ResetBits(GPIOB,SDA)        /*SDA做输入*/
#define SCL_H               GPIO_SetBits(GPIOB,SCL)          /*SCL做输出*/
#define SCL_L               GPIO_ResetBits(GPIOB,SCL)        /*SCL做输入*/
#define SDA_READ            GPIO_ReadInputDataBit(GPIOB,SDA) /* 读取SDA*/




//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
u16 IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(void);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 


//额外函数
void IIC_ADD_write(u8 DeviceAddr,u8 address,u8 Bytes);
u8 IIC_ADD_read(u8 DeviceAddr,u8 address);
u8 IIC_Read_MultiBytes(u8 DeviceAddr,u8 address,u8 Len,u8 *data);
void IIC_NoAddr_WriteByte(unsigned char address,unsigned char Bytes);

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);


#endif

//------------------End of File----------------------------

