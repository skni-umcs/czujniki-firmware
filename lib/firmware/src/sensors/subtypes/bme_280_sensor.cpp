#include "bme_280_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <string>
#include <sstream>
#include <iostream>
#include "bme_constants.h"
#include <utils/logger.h>

#if defined(esp32firebeetle)
  #define I2C_SDA 21
  #define I2C_SCL 22
#else
  #define I2C_SDA 8
  #define I2C_SCL 9
#endif

OperationResult BME280Sensor::setupSensor() {
  Wire.begin(I2C_SDA, I2C_SCL);

  unsigned status;

  status = bme.begin(0x76);  
  if (!status) {
    Logger::log("Could not find a valid BME280 sensor, address, sensor ID!");
    Logger::log("SensorID was: 0x"); Logger::log(bme.sensorID(),16);
    Logger::log("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Logger::log("   ID of 0x56-0x58 represents a BMP 280,\n");
    Logger::log("        ID of 0x60 represents a BME 280.\n");
    Logger::log("        ID of 0x61 represents a BME 680.\n");
    return OperationResult::ERROR;
  }
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> BME280Sensor::getSensorData() {
  Logger::log("Start BME280 reading");
  sensors_event_t event;
  std::stringstream result;

  Logger::log("BME HUMIDITY READ");
  int humidity = bme.readHumidity()*HUMIDITY_PRECISION;
  Logger::log("BME TEMPERATURE READ");
  int temperature = bme.readTemperature()*TEMPERATURE_PRECISION;
  Logger::log("BME PRESSURE READ");
  int pressure = bme.readPressure()/PRESSURE_CUT;
  Logger::log("BME FINISH READS");

  std::map<std::string, std::string> resultMap;
  Logger::log("BME INSERT HUMIDITY");
  resultMap.insert(std::make_pair(HUMIDITY_CODE, std::to_string(humidity)));
  Logger::log("BME INSERT TEMPERATURE");
  resultMap.insert(std::make_pair(TEMPERATURE_CODE, std::to_string(temperature)));\
  Logger::log("BME INSERT PRESSURE");
  resultMap.insert(std::make_pair(PRESSURE_CODE, std::to_string(pressure)));
  Logger::log("BME INSERT FINISHED");

  Logger::log("Finish BME280 reading");
  return resultMap;
}
