#include "sht20.h"
/*
*/
float send_to_sht20(uint8_t command)
{
	uint8_t recv_buff[2] = {0};//接收读取的前16位数据
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&hi2c1,WRIT_SHT20,&command,1,100);//调用HAL库函数
	if(ret==HAL_OK)
	{
		HAL_Delay(85);//至少延迟85ms
		HAL_I2C_Master_Receive(&hi2c1,READ_SHT20,recv_buff,2,100);
		
		uint16_t val = recv_buff[0]<<8|recv_buff[1];//两个8位的数据合并成16位的数据并返回
		return val;
	}
	return -1;//失败返回-1
}
/*

*/
void read_T_RH(float *TT,float *RRH)
{
	//先读取温度
	float T,RH;
	T = send_to_sht20(T_CMD);
	RH = send_to_sht20(RH_CMD);
	float T_val,RH_val;
	
	*TT = 175.72*T/65536-46.85;
	*RRH = 125*RH/65536-6;
	
}

// 


