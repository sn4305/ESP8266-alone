#include "driver/bh1750.h"

float result_lx=0;
u8 BUF[2]={0};
u16 result=0;

//��ʼ��
void Init_BH1750(void)
{
	;
}

void Cmd_Write_BH1750(u8 cmd)
{
	u8t error; // error code

	I2C_START();                  //��ʼ�ź�
	error  = I2C_TX(BH1750_Addr+0);   //�����豸��ַ+д�ź�
	if(error == NO_ERROR)  error  = I2C_TX(cmd);    //�ڲ��Ĵ�����ַ
	if(error == NO_ERROR)
	{
		//BH1750_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
		I2C2_Stop();                   //����ֹͣ�ź�
		delay_ms(5);
	}
}

void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_RSET);	//clear
	Cmd_Write_BH1750(BH1750_ONE);  //һ��H�ֱ���ģʽ������120ms��֮���Զ��ϵ�ģʽ  
}
void Read_BH1750(void)
{   	
	u8t error; // error code
	I2C_START();                          //��ʼ�ź�
	error  = I2C_TX(BH1750_Addr+1);         //�����豸��ַ+���ź�
	if(error == NO_ERROR)
	{
		BUF[0]=I2C_RX(ACK);  //����ACK
		BUF[1]=I2C_RX(NACK);  //����NACK

		I2C2_Stop();                          //ֹͣ�ź�
		delay_ms(5);
	}
}
void Convert_BH1750(void)
{
	
	result=BUF[0];
	result=(result<<8)+BUF[1];  //�ϳ����ݣ�����������
	
	result_lx=(float)result/1.2;

}


