#include "wifi.h"
#include <string.h>
#include <stdio.h>
#include "sht20.h"

#define RX_BUFFER_SIZE 512

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_index = 0;
uint8_t rx_data;
volatile uint8_t response_received = 0;

extern UART_HandleTypeDef huart5;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5)
    {
        if (rx_index < RX_BUFFER_SIZE - 1)
        {
            rx_buffer[rx_index++] = rx_data;
            rx_buffer[rx_index] = '\0';

            // 🔍 打印每个接收到的字符（调试用）
            printf("%c", rx_data);

            // 判断是否收到完整响应
            if (strstr((char *)rx_buffer, "OK") || strstr((char *)rx_buffer, "ERROR"))
            {
                response_received = 1;
            }
        }
        else
        {
            // 缓冲区满，清空处理
            rx_index = 0;
            memset(rx_buffer, 0, RX_BUFFER_SIZE);
        }

        // ✅ 继续接收下一个字节
        HAL_UART_Receive_IT(&huart5, &rx_data, 1);
    }
}

uint8_t ESP_WaitForResponse(uint32_t timeout_ms)
{
    uint32_t tickstart = HAL_GetTick();
    while (!response_received)
    {
        if ((HAL_GetTick() - tickstart) > timeout_ms)
        {
            return 0; // 超时
        }
    }

    response_received = 0;
    rx_index = 0;
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    return 1; // 收到 OK 或 ERROR
}

uint8_t ESP_SendCmd(const char *cmd)
{
    char buffer[256];
    sprintf(buffer, "%s\r\n", cmd);
    HAL_UART_Transmit(&huart5, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    return ESP_WaitForResponse(3000); // 最多等待 3 秒

//char buffer[512];  // 👈 改为足够大的 buffer，避免溢出
//    sprintf(buffer, "%s\r\n", cmd);

//    printf("📤 发送指令: %s\n", buffer);  // 打印要发的指令

//    HAL_UART_Transmit(&huart5, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

//    return ESP_WaitForResponse(3000);  // 最多等待 3 秒响应
}

void ESP8266_Init(void)
{
    HAL_UART_Receive_IT(&huart5, &rx_data, 1);  // 启动中断接收

    ESP_SendCmd("AT+RST");
    HAL_Delay(1000);
    ESP_SendCmd("AT+CWMODE=1");
    HAL_Delay(1000);

    // 修改为你的 WiFi 名称和密码
    ESP_SendCmd("AT+CWJAP=\"red\",\"12345678\"");
    HAL_Delay(500);
}

void MQTT_Connect_Huawei(void)
{
//    ESP_SendCmd("AT+MQTTUSERCFG=0,1,\"68009a6cfde7ae3745986ec6_sht20_0_0_2025041706\","
//                "\"68009a6cfde7ae3745986ec6_sht20\","
//                "\"63760ca720bb96fc7499a8be241b5b6892b2a42dec4b92bda0bb8cf35dedc1e8\",0,0,\"\"");
ESP_SendCmd("AT+MQTTUSERCFG=0,1,\"68009a6cfde7ae3745986ec6_sht20_0_0_2025042202\","
                "\"68009a6cfde7ae3745986ec6_sht20\","
                "\"bba9acb4e149c1b3290b64285862eef5bc10ed772127ce96afd0a1b1f8f0e315\",0,0,\"\"");

    HAL_Delay(9000);

    ESP_SendCmd("AT+MQTTCONN=0,\"a56f4d6137.st1.iotda-device.cn-east-3.myhuaweicloud.com\",1883,1\r\n");
  //ESP_SendCmd("AT+MQTTCONN=0,\"124.70.218.131\",1883,1\r\n");
    HAL_Delay(9000);
}


void MQTT_Publish_SensorData(float temp, float hum)
{
 const char *topic = "$oc/devices/68009a6cfde7ae3745986ec6_sht20/sys/properties/report";
    char payload[256];
    char cmd[512];

	   sprintf(cmd, "AT+MQTTPUBRAW=0,\"%s\",256,0,0", topic);

    ESP_SendCmd(cmd);
  sprintf(payload, "{\"services\":[{\"service_id\":\"sht20\",\"properties\":{\"temperature\":%.2f,\"hum\":%.2f}}]}",temp, hum);
    ESP_SendCmd(payload);
}


