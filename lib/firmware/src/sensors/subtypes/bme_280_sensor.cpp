#include "bme_280_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <string>
#include <sstream>
#include <iostream>

#if defined(esp32firebeetle)
  #define I2C_SDA 21
  #define I2C_SCL 22
#else
  #define I2C_SDA 8
  #define I2C_SCL 9
#endif

const std::string TEMPERATURE_CODE = "t";
const std::string PRESSURE_CODE = "p";
const std::string HUMIDITY_CODE = "h";

#define HUMIDITY_PRECISION 1;
#define TEMPERATURE_PRECISION 100;
#define PRESSURE_CUT 10;

Adafruit_BME280 bme;

OperationResult BME280Sensor::setupSensor() {
  Wire.begin(I2C_SDA, I2C_SCL);

  unsigned status;

  status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    return OperationResult::ERROR;
  }
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> BME280Sensor::getSensorData() {
  sensors_event_t event;
  std::stringstream result;

  int humidity = bme.readHumidity()*HUMIDITY_PRECISION;
  int temperature = bme.readTemperature()*TEMPERATURE_PRECISION;
  int pressure = bme.readPressure()/PRESSURE_CUT;

  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(HUMIDITY_CODE, std::to_string(humidity)));
  resultMap.insert(std::make_pair(TEMPERATURE_CODE, std::to_string(temperature)));
  resultMap.insert(std::make_pair(PRESSURE_CODE, std::to_string(pressure)));

  return resultMap;
}
