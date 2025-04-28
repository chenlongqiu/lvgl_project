/**
  ******************************************************************************
  * @file   bsp_ft6336.c
  * @brief  FT6336电容触摸屏驱动实现
  ******************************************************************************
  */
#include "bsp_ft6336.h"
#include "i2c.h"
#include "gpio.h"
#include <stdio.h>
/* 全局触摸设备实例 */
FT6336_Dev tp_dev = {
    .touch_num = 0,
    .points = {{0}},
    .pressed = 0
};

/* 私有函数声明 */
static uint16_t FT6336_ReadTouchXY(uint8_t reg);

/**
  * @brief  初始化FT6336触摸屏
  */
void FT6336_Init(void)
{
    /* 硬件复位（直接使用PA11） */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); // 拉低复位
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   // 拉高复位
    HAL_Delay(100);
    
    /* 验证芯片ID */
    uint8_t chip_id = FT6336_ReadReg(FT6336_REG_CHIP_ID);
    if(chip_id != 0x51) {
        printf("FT6336 Init Failed! Chip ID: 0x%02X\r\n", chip_id);
        return;
    }
    
    /* 配置触摸阈值 */
    FT6336_WriteReg(FT6336_REG_THRESHOLD, 0x28);
    
    printf("FT6336 Init OK! Chip ID: 0x%02X\r\n", chip_id);
}

/**
  * @brief  扫描触摸点数据
  */
void FT6336_Scan(void)
{
    uint8_t touch_num = FT6336_ReadReg(FT6336_REG_TD_STATUS) & 0x0F;
    tp_dev.touch_num = touch_num > 2 ? 0 : touch_num; // 最大支持2点
    
    if(tp_dev.touch_num == 0) {
        tp_dev.pressed = 0;
        for(uint8_t i = 0; i < 2; i++) {
            tp_dev.points[i].status = TOUCH_RELEASE;
        }
        return;
    }
    
    tp_dev.pressed = 1;
    
    /* 读取第一个触摸点 */
    tp_dev.points[0].x = FT6336_ReadTouchXY(FT6336_REG_TOUCH1_XH);
    tp_dev.points[0].y = FT6336_ReadTouchXY(FT6336_REG_TOUCH1_YH);
    tp_dev.points[0].id = FT6336_ReadReg(FT6336_REG_TOUCH1_XH) >> 4;
    tp_dev.points[0].status = TOUCH_PRESS;
    
    /* 如果有第二个触摸点 */
    if(tp_dev.touch_num > 1) {
        tp_dev.points[1].x = FT6336_ReadTouchXY(FT6336_REG_TOUCH2_XH);
        tp_dev.points[1].y = FT6336_ReadTouchXY(FT6336_REG_TOUCH2_YH);
        tp_dev.points[1].id = FT6336_ReadReg(FT6336_REG_TOUCH2_XH) >> 4;
        tp_dev.points[1].status = TOUCH_PRESS;
    } else {
        tp_dev.points[1].status = TOUCH_RELEASE;
    }
}

/**
  * @brief  读取触摸坐标值
  * @param  reg: 寄存器地址
  * @retval 坐标值
  */
static uint16_t FT6336_ReadTouchXY(uint8_t reg)
{
    uint8_t buf[2];
    buf[0] = FT6336_ReadReg(reg);
    buf[1] = FT6336_ReadReg(reg + 1);
    return ((buf[0] & 0x0F) << 8) | buf[1];
}

/**
  * @brief  读取寄存器值
  * @param  reg: 寄存器地址
  * @retval 寄存器值
  */
uint8_t FT6336_ReadReg(uint8_t reg)
{
    uint8_t value = 0;
    HAL_I2C_Master_Transmit(&hi2c1, FT6336_ADDR_WRITE, &reg, 1, 100);
    HAL_Delay(1);
    HAL_I2C_Master_Receive(&hi2c1, FT6336_ADDR_READ, &value, 1, 100);
    return value;
}

/**
  * @brief  写入寄存器值
  * @param  reg: 寄存器地址
  * @param  value: 要写入的值
  */
void FT6336_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    HAL_I2C_Master_Transmit(&hi2c1, FT6336_ADDR_WRITE, buf, 2, 100);
}

/**
  * @brief  触摸中断处理函数
  */
void FT6336_IRQHandler(void)
{
    FT6336_Scan();
    
    /* 调试输出 */
    if(tp_dev.pressed) {
        printf("Touch: X=%d, Y=%d\r\n", tp_dev.points[0].x, tp_dev.points[0].y);
    }
}