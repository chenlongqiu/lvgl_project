#ifndef __WIFI_H
#define __WIFI_H

#include "stm32u5xx_hal.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

void ESP8266_Init(void);
void MQTT_Connect_Huawei(void);
//void MQTT_Publish_Temp(void);
void MQTT_Publish_SensorData(float temp, float hum);



uint8_t ESP_SendCmd(const char *cmd);
uint8_t ESP_WaitForResponse(uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
