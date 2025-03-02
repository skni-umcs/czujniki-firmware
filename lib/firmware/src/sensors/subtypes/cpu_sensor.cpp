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

const std::string CPU_TEMPERATURE_CODE = "c";

OperationResult CPUSensor::setupSensor() {
  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
  temp_sensor_get_config(&temp_sensor);
  Serial.printf("default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
  temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
  temp_sensor_set_config(temp_sensor);
  temp_sensor_start();
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> CPUSensor::getSensorData() {
  float temperature;
  temp_sensor_read_celsius(&temperature);

  int convertedTemperature = temperature*100;

  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(CPU_TEMPERATURE_CODE, std::to_string(convertedTemperature)));
  return resultMap;
}
#else //other boards dont have temperature sensor
OperationResult CPUSensor::setupSensor() {
  return OperationResult::NOT_FOUND;
}

std::map<std::string, std::string> CPUSensor::getSensorData() {
  std::map<std::string, std::string> resultMap;
  return resultMap;
}
#endif