#include "sensor_facade_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include <sstream>

const std::string TELEMETRY_PERIOD_MS = "t";
const std::string SERVICE_PERIOD_MS = "y";

SensorFacadeSensor::SensorFacadeSensor(std::shared_ptr<SensorFacade> facade) {
  this->facade = facade;
}

OperationResult SensorFacadeSensor::setupSensor() {
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> SensorFacadeSensor::getSensorData() {
  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(TELEMETRY_PERIOD_MS, std::to_string(facade->getTelemetryPeriodMs())));
  resultMap.insert(std::make_pair(SERVICE_PERIOD_MS, std::to_string(facade->getServicePeriodMs())));
  return resultMap;
}