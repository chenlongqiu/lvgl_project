#include "sht20.h"
/*
*/
float send_to_sht20(uint8_t command)
{
	uint8_t recv_buff[2] = {0};//���ն�ȡ��ǰ16λ����
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&hi2c1,WRIT_SHT20,&command,1,100);//����HAL�⺯��
	if(ret==HAL_OK)
	{
		HAL_Delay(85);//�����ӳ�85ms
		HAL_I2C_Master_Receive(&hi2c1,READ_SHT20,recv_buff,2,100);
		
		uint16_t val = recv_buff[0]<<8|recv_buff[1];//����8λ�����ݺϲ���16λ�����ݲ�����
		return val;
	}
	return -1;//ʧ�ܷ���-1
}
/*

*/
void read_T_RH(float *TT,float *RRH)
{
	//�ȶ�ȡ�¶�
	float T,RH;
	T = send_to_sht20(T_CMD);
	RH = send_to_sht20(RH_CMD);
	float T_val,RH_val;
	
	*TT = 175.72*T/65536-46.85;
	*RRH = 125*RH/65536-6;
	
}

// 


