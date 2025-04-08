#include "heap_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include <sstream>

static const char *TAG = "TempSensor";

const std::string HEAP_CODE = "heap";
const std::string INTERNAL_HEAP_CODE = "intern_heap";

OperationResult HeapSensor::setupSensor() {
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> HeapSensor::getSensorData() {
  size_t free_heap = esp_get_free_heap_size();
  size_t free_internal_heap = esp_get_free_internal_heap_size();

  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(HEAP_CODE, std::to_string(free_heap)));
  resultMap.insert(std::make_pair(INTERNAL_HEAP_CODE, std::to_string(free_internal_heap)));
  return resultMap;
}