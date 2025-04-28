#include "ap3216c.h"

int  send_AP32(uint8_t REG_ADD,uint8_t COMMAND)
{
	HAL_StatusTypeDef ret;
	ret= HAL_I2C_Mem_Write(&hi2c1,W_ADDR,REG_ADD,1,&COMMAND,1,100);
	if (ret!=HAL_OK)
	{
			return 0;
	}
	return -1;	
//I2C_HandleTypeDef *hi2c, 参数1：I2C句柄
//uint16_t DevAddress, 参数2：从机地址
//uint16_t MemAddress,参数3：寄存器地址
//uint16_t MemAddSize, 参数4：寄存器地址大小
//uint8_t *pData, 参数5：接收的传感器数据
//uint16_t Size,参数6：数据大小
//uint32_t Timeout)参数7：超时时间
	
}
void AP32_init()
{
	send_AP32(0x00,0x04);//软复位
	send_AP32(0x00,0x03);//开启所有功能
	HAL_Delay(100);
}

uint8_t read_AP32(uint8_t REG_ADDR)
{
	uint8_t val;
	HAL_StatusTypeDef ret;
	ret=HAL_I2C_Mem_Read(&hi2c1,W_ADDR,REG_ADDR,1,&val,1,100);
	if(ret!=HAL_OK)
	{
		return 0;
	}
	return val;
}
	
void Get_val(uint16_t *IR ,uint16_t *PS ,uint16_t *ALS)
{
	uint8_t val_H,val_L;
	
	val_L=read_AP32(0x0A);
	val_H=read_AP32(0x0B);
	*IR= (val_L&0x03) |val_H<<2;
	
	val_L=read_AP32(0x0C);
	val_H=read_AP32(0x0D);
	*ALS= val_L |val_H<<8;
	
	val_L=read_AP32(0x0E);
	val_H=read_AP32(0x0F);
	*PS= (val_L&0x0F) |((val_H&0x3F)<<4);
}




	