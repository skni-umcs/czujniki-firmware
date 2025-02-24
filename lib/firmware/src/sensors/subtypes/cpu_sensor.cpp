#include "cpu_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include <sstream>
#if defined(production) || defined(mini_test)
#include "driver/temp_sensor.h"

static const char *TAG = "TempSensor";

void CPUSensor::setupSensor(uint32_t* delayMS) {
  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
  temp_sensor_get_config(&temp_sensor);
  Serial.printf("default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
  temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
  temp_sensor_set_config(temp_sensor);
  temp_sensor_start();
}

std::string CPUSensor::getSensorDataJson() {
  float temperature;
  temp_sensor_read_celsius(&temperature);

  std::stringstream result;
  result << "CPU temperature: " << temperature;
  return result.str();
}
#else //other boards dont have temperature sensor
void CPUSensor::setupSensor(uint32_t* delayMS) {}

std::string CPUSensor::getSensorData() {
  return "";
}
#endif