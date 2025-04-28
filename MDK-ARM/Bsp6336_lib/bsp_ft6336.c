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

//
volatile uint32_t ft6336_on_touch_count = 0;

//event_notify_cb touch_notify;
FT6336_TouchPointType tp;

/*
**********************************************************************
* @fun     :FT6336_readByte 
* @brief   :读取从机数据
* @param   :addr:寄存器地址
* @return  :data:返回的数据
**********************************************************************
*/
uint8_t FT6336_readByte(uint8_t addr)
{
    uint8_t data;
		//发送控制指令
		HAL_I2C_Master_Transmit(&hi2c1,FT6336_ADDR_WRITE,&addr,1,100);
		//适当增加延时，等待设置完成
		HAL_Delay(1);
		//读取坐标数据，一个字节
		HAL_I2C_Master_Receive(&hi2c1,FT6336_ADDR_READ,&data,1,100);	  
    return data;
}
/*
**********************************************************************
* @fun     :FT6336_writeByte 
* @brief   :对从机写数据
* @param   :addr:寄存器地址，data:写入的数据
* @return  :None 
**********************************************************************
*/
void FT6336_writeByte(uint8_t addr, uint8_t data)
{
		HAL_I2C_Master_Transmit(&hi2c1,FT6336_ADDR_WRITE,&data,1,100);
}
/*
**********************************************************************
* @fun     :FT6336_irq_fuc 
* @brief   :中断函数调用，生产按压事件
* @param   :None
* @return  :None 
**********************************************************************
*/
void FT6336_irq_fuc(void)
{
    ft6336_on_touch_count = 1;
}
/*
**********************************************************************
* @fun     :FT6336_scan_task 
* @brief   :触摸事件，扫描触摸点
* @param   :None
* @return  :None 
**********************************************************************
*/
void FT6336_scan_task(void)
{
    if (ft6336_on_touch_count) // irq done
    {
        tp = FT6336_scan();
        if (tp.tp[0].status == 1)
        {                                  //最多支持两个触点
//					printf("touch down x %d  y %d\r\n", tp.tp[0].x, tp.tp[0].y);
        }
        ft6336_on_touch_count = 0;
    }
}
/*
**********************************************************************
* @fun     :FT6336_init 
* @brief   :FT6336初始化函数
* @param   :None
* @return  :None 
**********************************************************************
*/
void FT6336_init(void)
{
    // Int Pin Configuration
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		HAL_Delay(100);
}
/*
**********************************************************************
* @fun     :FT6336状态读取函数
**********************************************************************
*/
uint8_t FT6336_read_device_mode(void)
{
    return (FT6336_readByte(FT6336_ADDR_DEVICE_MODE) & 0x70) >> 4;
}
//
void FT6336_write_device_mode(DEVICE_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_DEVICE_MODE, (mode & 0x07) << 4);
}
//
uint8_t FT6336_read_gesture_id(void)
{
    return FT6336_readByte(FT6336_ADDR_GESTURE_ID);
}
//
uint8_t FT6336_read_td_status(void)
{
    return FT6336_readByte(FT6336_ADDR_TD_STATUS);
}
//
uint8_t FT6336_read_touch_number(void)
{
    return FT6336_readByte(FT6336_ADDR_TD_STATUS) & 0x0F;
}
/*
**********************************************************************
* @fun     :FT6336_read_touch1_x 
* @brief   :读取touch1的数据
* @param   :None
* @return  :None 
**********************************************************************
*/
uint16_t FT6336_read_touch1_x(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH1_X);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH1_X + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
//
uint16_t FT6336_read_touch1_y(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH1_Y);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH1_Y + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
//
uint8_t FT6336_read_touch1_event(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_EVENT) >> 6;
}
//
uint8_t FT6336_read_touch1_id(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_ID) >> 4;
}
//
uint8_t FT6336_read_touch1_weight(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_WEIGHT);
}
//
uint8_t FT6336_read_touch1_misc(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_MISC) >> 4;
}
/*
**********************************************************************
* @fun     :FT6336_read_touch2_x 
* @brief   :读取touch2的数据
* @param   :None
* @return  :None 
**********************************************************************
*/
uint16_t FT6336_read_touch2_x(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH2_X);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH2_X + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
//
uint16_t FT6336_read_touch2_y(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH2_Y);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH2_Y + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
//
uint8_t FT6336_read_touch2_event(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_EVENT) >> 6;
}
//
uint8_t FT6336_read_touch2_id(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_ID) >> 4;
}
//
uint8_t FT6336_read_touch2_weight(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_WEIGHT);
}
//
uint8_t FT6336_read_touch2_misc(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_MISC) >> 4;
}
/*
**********************************************************************
* @fun     :Mode Parameter Register
**********************************************************************
*/
uint8_t FT6336_read_touch_threshold(void)
{
    return FT6336_readByte(FT6336_ADDR_THRESHOLD);
}
//
uint8_t FT6336_read_filter_coefficient(void)
{
    return FT6336_readByte(FT6336_ADDR_FILTER_COE);
}
//
uint8_t FT6336_read_ctrl_mode(void)
{
    return FT6336_readByte(FT6336_ADDR_CTRL);
}
//
void FT6336_write_ctrl_mode(CTRL_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_CTRL, mode);
}
//
uint8_t FT6336_read_time_period_enter_monitor(void)
{
    return FT6336_readByte(FT6336_ADDR_TIME_ENTER_MONITOR);
}
//
uint8_t FT6336_read_active_rate(void)
{
    return FT6336_readByte(FT6336_ADDR_ACTIVE_MODE_RATE);
}
//
uint8_t FT6336_read_monitor_rate(void)
{
    return FT6336_readByte(FT6336_ADDR_MONITOR_MODE_RATE);
}
/*
**********************************************************************
* @fun     :Gesture Parameters
**********************************************************************
*/
uint8_t FT6336_read_radian_value(void)
{
    return FT6336_readByte(FT6336_ADDR_RADIAN_VALUE);
}
//
void FT6336_write_radian_value(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_RADIAN_VALUE, val);
}
//
uint8_t FT6336_read_offset_left_right(void)
{
    return FT6336_readByte(FT6336_ADDR_OFFSET_LEFT_RIGHT);
}
//
void FT6336_write_offset_left_right(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_OFFSET_LEFT_RIGHT, val);
}
//
uint8_t FT6336_read_offset_up_down(void)
{
    return FT6336_readByte(FT6336_ADDR_OFFSET_UP_DOWN);
}
//
void FT6336_write_offset_up_down(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_OFFSET_UP_DOWN, val);
}
//
uint8_t FT6336_read_distance_left_right(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_LEFT_RIGHT);
}
//
void FT6336_write_distance_left_right(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_LEFT_RIGHT, val);
}
//
uint8_t FT6336_read_distance_up_down(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_UP_DOWN);
}
//
void FT6336_write_distance_up_down(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_UP_DOWN, val);
}
//
uint8_t FT6336_read_distance_zoom(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_ZOOM);
}
//
void FT6336_write_distance_zoom(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_ZOOM, val);
}
/*
**********************************************************************
* @fun     :System Information
**********************************************************************
*/
uint16_t FT6336_read_library_version(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_LIBRARY_VERSION_H);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_LIBRARY_VERSION_L);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
//
uint8_t FT6336_read_chip_id(void)
{
    return FT6336_readByte(FT6336_ADDR_CHIP_ID);
}
//
uint8_t FT6336_read_g_mode(void)
{
    return FT6336_readByte(FT6336_ADDR_G_MODE);
}
//
void FT6336_write_g_mode(G_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_G_MODE, mode);
}
//
uint8_t FT6336_read_pwrmode(void)
{
    return FT6336_readByte(FT6336_ADDR_POWER_MODE);
}
//
uint8_t FT6336_read_firmware_id(void)
{
    return FT6336_readByte(FT6336_ADDR_FIRMARE_ID);
}
//
uint8_t FT6336_read_focaltech_id(void)
{
    return FT6336_readByte(FT6336_ADDR_FOCALTECH_ID);
}
//
uint8_t FT6336_read_release_code_id(void)
{
    return FT6336_readByte(FT6336_ADDR_RELEASE_CODE_ID);
}
//
uint8_t FT6336_read_state(void)
{
    return FT6336_readByte(FT6336_ADDR_STATE);
}
/*
**********************************************************************
* @fun     :FT6336_scan 
* @brief   :FT6336触摸点读取
* @param   :None
* @return  :触摸点信息
**********************************************************************
*/
FT6336_TouchPointType FT6336_scan(void)
{
    FT6336_TouchPointType touchPoint;
		//
    touchPoint.touch_count = FT6336_read_td_status();
		//
    if (touchPoint.touch_count == 0)
    {
        touchPoint.tp[0].status = release;
        touchPoint.tp[1].status = release;
    }
		//
    uint8_t id1 = FT6336_read_touch1_id(); // id1 = 0 or 1
    touchPoint.tp[id1].status = (touchPoint.tp[id1].status == release) ? touch : stream;
    touchPoint.tp[id1].x = FT6336_read_touch1_x();
    touchPoint.tp[id1].y = FT6336_read_touch1_y();
    touchPoint.tp[~id1 & 0x01].status = release;
		//
    return touchPoint;
}