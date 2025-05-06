/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"
#include "semphr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"


#include "bsp_ili9341_4line.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "bsp_ft6336.h"
#include "ui.h"
#include "sht20.h"
#include "wifi.h"
#include <stdlib.h> 
#include "stdarg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t RecvBuf[2048];
extern uint8_t buf[128];
uint8_t buf2[128];
extern lv_chart_series_t * existing_series; // 用于存储已有曲线的指针
extern int flag;
extern float T,RH;	//温湿度	
static SemaphoreHandle_t xMutex;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for lvgl_task_handler */
osThreadId_t lvgl_task_handlerHandle;
const osThreadAttr_t lvgl_task_handler_attributes = {
  .name = "lvgl_task_handler",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for lvgl_refresh_task */
osThreadId_t lvgl_refresh_taskHandle;
const osThreadAttr_t lvgl_refresh_task_attributes = {
  .name = "lvgl_refresh_task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//修改chart2============
void setup_chart_axis() {
    // 设置纵坐标范围
    lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 40, 90);

    // 设置 X、Y 轴的偏移量
    lv_obj_set_pos(ui_Chart2, 5, -8);
}
/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
	
	// 告诉lvgl已经过去了1毫秒
	lv_tick_inc(1);
}
/* USER CODE END 3 */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of lvgl_task_handler */
  lvgl_task_handlerHandle = osThreadNew(lvgl_task_handler, NULL, &lvgl_task_handler_attributes);

  /* creation of lvgl_refresh_task */
  lvgl_refresh_taskHandle = osThreadNew(lvgl_refresh_task, NULL, &lvgl_refresh_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_lvgl_task_handler */
/**
* @brief Function implementing the lvgl_task_handler thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lvgl_task_handler */
void lvgl_task_handler(void *argument)
{
  /* USER CODE BEGIN lvgl_task_handler */
  /* Infinite loop */
	xMutex = xSemaphoreCreateMutex();
  for(;;)
  {
		//=====调用刷新函数==================

		if(pdTRUE == xSemaphoreTake(xMutex,portMAX_DELAY))	
		{
			lv_timer_handler();		
			xSemaphoreGive(xMutex);
		}
		vTaskDelay(pdMS_TO_TICKS(1));
  }
  /* USER CODE END lvgl_task_handler */
}

/* USER CODE BEGIN Header_lvgl_refresh_task */
/**
* @brief Function implementing the lvgl_refresh_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lvgl_refresh_task */
void lvgl_refresh_task(void *argument)
{
  /* USER CODE BEGIN lvgl_refresh_task */
  /* Infinite loop */
  for(;;)
  {	
		// 设置图表的坐标轴范围和偏移量
	setup_chart_axis();
	//=========================================		
	// 获取已有曲线
	existing_series = lv_chart_get_series_next(ui_Chart2, NULL);
	if (existing_series == NULL) {
			// 若未找到曲线，可进行错误处理
			printf("Error: No series found on the chart.\n");
	}

	//=====================================
	read_T_RH(&T,&RH);//调用函数
	sprintf((char*)buf,"tem=%.2f   hum=%.2f\n",T,RH);//输出的内容
  
	//=============将温度上传到屏幕============		
		
		// 格式化温度数据字符串
    char tempBuf[16];
    sprintf(tempBuf, "%.2f", T);
    // 更新温度标签的文本
    lv_label_set_text(ui_Label7, tempBuf);

    // 格式化湿度数据字符串
    char humBuf[16];
    sprintf(humBuf, "%.2f", RH);
    // 更新湿度标签的文本
    lv_label_set_text(ui_Label8, humBuf);
		//=======================================	
		//湿度折线图
		// 添加湿度数据到折线图
    if (existing_series != NULL) {
        // 添加湿度数据到曲线
        lv_chart_set_next_value(ui_Chart2, existing_series, (lv_coord_t)(RH)); // 乘以10以处理小数部分
        // 更新折线图显示
        lv_chart_refresh(ui_Chart2);
    }
		
//================================================		
		
	// ★ 温湿度控制逻辑
		
		if(T > 27.2) // 使用变量阈值
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);   // 打开风扇
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET); // 关闭风扇

        if(RH >80.0)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);   // 打开马达
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // 关闭马达

		// 上传数据到云平台
		MQTT_Publish_SensorData(T, RH);
		if(flag==1)
		{
			printf("transmit succeed:%s\n\r",buf);
			flag=0;
		}
  } 
  osDelay(pdMS_TO_TICKS(500));
  /* USER CODE END lvgl_refresh_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

