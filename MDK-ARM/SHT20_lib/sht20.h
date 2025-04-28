/*

*/
#ifndef __SHT20_H__
#define __SHT20_H__
#include "i2c.h"
#include "stdio.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_uart.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "bsp_ili9341_4line.h"
#define ADDERSS 0x40//7λ��SHT20�豸�ӻ���ַ�����������ַ��
/*
0x40===>0100 0000==����һλ==>1000 000==���϶�λ==>1000 0001
==����дλ==>1000 0000
*/

#define READ_SHT20 0x81 //��
#define WRIT_SHT20 0x80 //д

#define T_CMD 0xF3 //�����¶ȵ�����
#define RH_CMD 0xF5//����ʪ�ȵ�����
//1������1���������sht20�������������Ϊ�¶Ⱥ�ʪ�ȣ�
float send_to_sht20(uint8_t command);
//2������2���ú���1�����պ���1�ķ���ֵ����ȡ���¶Ⱥ�ʪ�ȣ�
void read_T_RH(float *TT,float *RRH);

#endif

