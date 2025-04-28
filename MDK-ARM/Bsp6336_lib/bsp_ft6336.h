/**
  ******************************************************************************
  * @file   bsp_ft6336.h
  * @brief  2.8寸屏FT6336驱动文件，I2C接口
  ******************************************************************************
  */
#ifndef __BSP_FT6336_H__
#define __BSP_FT6336_H__

#include "main.h"
#include <stdint.h>

/* I2C 地址定义 */
#define FT6336_I2C_ADDR         0x38
#define FT6336_ADDR_READ        0x71
#define FT6336_ADDR_WRITE       0x70

/* 触摸状态枚举 */
typedef enum {
    TOUCH_RELEASE = 0,    // 未触摸
    TOUCH_PRESS,          // 按下
    TOUCH_CONTACT         // 持续接触
} TouchStatus;

/* 触摸点数据结构 */
typedef struct {
    uint16_t x;           // X坐标
    uint16_t y;           // Y坐标
    TouchStatus status;   // 触摸状态
    uint8_t id;           // 触摸点ID
} TouchPoint;

/* 触摸设备全局结构体 */
typedef struct {
    uint8_t touch_num;    // 当前触摸点数
    TouchPoint points[2]; // 支持两点触摸
    uint8_t pressed;      // 全局按压状态
} FT6336_Dev;

extern FT6336_Dev tp_dev; // 全局触摸设备实例

/* 寄存器地址定义 */
#define FT6336_REG_DEVICE_MODE         0x00
#define FT6336_REG_GESTURE_ID          0x01
#define FT6336_REG_TD_STATUS           0x02
#define FT6336_REG_TOUCH1_XH           0x03
#define FT6336_REG_TOUCH1_XL           0x04
#define FT6336_REG_TOUCH1_YH           0x05
#define FT6336_REG_TOUCH1_YL           0x06
#define FT6336_REG_TOUCH2_XH           0x09
#define FT6336_REG_TOUCH2_XL           0x0A
#define FT6336_REG_TOUCH2_YH           0x0B
#define FT6336_REG_TOUCH2_YL           0x0C
#define FT6336_REG_THRESHOLD           0x80
#define FT6336_REG_CHIP_ID             0xA3
#define FT6336_REG_POWER_MODE          0xA5

/* 函数声明 */
void FT6336_Init(void);
void FT6336_Scan(void);
uint8_t FT6336_ReadReg(uint8_t reg);
void FT6336_WriteReg(uint8_t reg, uint8_t value);
void FT6336_IRQHandler(void);

/* 触摸点获取函数 */
static inline uint8_t FT6336_GetTouchNum(void) { return tp_dev.touch_num; }
static inline uint16_t FT6336_GetX(uint8_t touch_id) { return tp_dev.points[touch_id].x; }
static inline uint16_t FT6336_GetY(uint8_t touch_id) { return tp_dev.points[touch_id].y; }
static inline uint8_t FT6336_IsPressed(void) { return tp_dev.pressed; }

#endif /* __BSP_FT6336_H__ */