#include "driver/bh1750.h"

float result_lx=0;
u8 BUF[2]={0};
u16 result=0;

//初始化
void Init_BH1750(void)
{
	;
}

void Cmd_Write_BH1750(u8 cmd)
{
	u8t error; // error code

	I2C_START();                  //起始信号
	error  = I2C_TX(BH1750_Addr+0);   //发送设备地址+写信号
	if(error == NO_ERROR)  error  = I2C_TX(cmd);    //内部寄存器地址
	if(error == NO_ERROR)
	{
		//BH1750_SendByte(REG_data);       //内部寄存器数据，
		I2C2_Stop();                   //发送停止信号
		delay_ms(5);
	}
}

void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_RSET);	//clear
	Cmd_Write_BH1750(BH1750_ONE);  //一次H分辨率模式，至少120ms，之后自动断电模式  
}
void Read_BH1750(void)
{   	
	u8t error; // error code
	I2C_START();                          //起始信号
	error  = I2C_TX(BH1750_Addr+1);         //发送设备地址+读信号
	if(error == NO_ERROR)
	{
		BUF[0]=I2C_RX(ACK);  //发送ACK
		BUF[1]=I2C_RX(NACK);  //发送NACK

		I2C2_Stop();                          //停止信号
		delay_ms(5);
	}
}
void Convert_BH1750(void)
{
	
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据
	
	result_lx=(float)result/1.2;

}


