/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "icache.h"
#include "memorymap.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>    // 标准库
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

/* USER CODE BEGIN PV */
uint8_t RecvBuf[2048];
	uint8_t buf[128];
	uint8_t buf2[128];
//	float temp_threshold = 29.0;  // 初始温度阈值
//	uint8_t threshold_str[32];       // 添加阈值显示字符串缓冲区
lv_chart_series_t * existing_series; // 用于存储已有曲线的指针


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *stream)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
    return ch;
}

int flag=0;

//修改chart2============
void setup_chart_axis() {
    // 设置纵坐标范围
    lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 45, 90);

    // 设置 X、Y 轴的偏移量
    lv_obj_set_pos(ui_Chart2, 5, -8);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
	
//===========以下是屏幕的初始化================
//	ILI9341_Clear(CYAN);
	lv_init();//lvgl初始化
	lv_port_disp_init();//显示屏初始化(初始化驱动程序）
	lv_port_indev_init();//触摸屏初始化FT6336_Init();
	
	HAL_TIM_Base_Start_IT(&htim6);//开启定时器中断
	
	// 调用SquareLine生成的界面初始化函数
    ui_init();
		// 设置图表的坐标轴范围和偏移量
		setup_chart_axis();
		
//=========================================		
		// 获取已有曲线
existing_series = lv_chart_get_series_next(ui_Chart2, NULL);
if (existing_series == NULL) {
    // 若未找到曲线，可进行错误处理
    printf("Error: No series found on the chart.\n");
}
		
//===========================================	

//=========以下是温湿度，wifi=====================	
	uint16_t IR,PS,ALS;//ap3216c的数据
	float T,RH;	//温湿度
	
	ESP8266_Init(); // 初始化模块和WiFi
	MQTT_Connect_Huawei();// 连接到华为云平台
	printf("connect succeed\n\r");
	//1、开启串口5的接收中断
    HAL_UART_Receive_IT(&huart5,buf,sizeof(buf));
    //2、开启串口5的空闲中断（接收ESP8266返回的不定长信息）
    __HAL_UART_ENABLE_IT(&huart5,UART_IT_IDLE);
//=============================================	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		 /* USER CODE END WHILE */
		
		 /* USER CODE BEGIN 3 */
		
//=====调用刷新函数==================
			lv_timer_handler();			
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
		
		if(T > 29) // 使用变量阈值
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
    HAL_Delay(1000);
  } 
  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 {
 if(htim->Instance==TIM6)
 {
 lv_tick_inc(1);
 static uint16_t ledTimes=0;
 if(ledTimes++ >= 500)
 {
 HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
 ledTimes=0;
 }
 }
 }
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
