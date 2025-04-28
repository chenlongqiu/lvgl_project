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
#define ADDERSS 0x40//7位的SHT20设备从机地址（不用这个地址）
/*
0x40===>0100 0000==左移一位==>1000 000==加上读位==>1000 0001
==加上写位==>1000 0000
*/

#define READ_SHT20 0x81 //读
#define WRIT_SHT20 0x80 //写

#define T_CMD 0xF3 //测量温度的命令
#define RH_CMD 0xF5//测量湿度的命令
//1、函数1发送命令给sht20传感器（命令分为温度和湿度）
float send_to_sht20(uint8_t command);
//2、函数2调用函数1，接收函数1的返回值（读取的温度和湿度）
void read_T_RH(float *TT,float *RRH);

#endif

