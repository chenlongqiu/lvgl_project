//1.初始化，开启所有功能
//2.读取数据
#ifndef AP_32_H
#define AP_32_H

#include "i2c.h"

#define ADDRESS 0x1E  //没有用
#define W_ADDR 0x3C//写位
#define R_ADDR 0x3D//读位
#define SYSTEM 0x00
#define SW_RESET 0x04
#define START_ALL 0x03

void Get_val(uint16_t *IR ,uint16_t *PS ,uint16_t *ALS);
void AP32_init();
uint8_t read_AP32(uint8_t REG_ADDR);
#endif 

