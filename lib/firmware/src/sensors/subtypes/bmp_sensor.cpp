#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "bmp_sensor.h"
#include <sstream>
#include <utility>

// Define I2C pins
#if defined(esp32firebeetle)
  #define I2C_SDA 21
  #define I2C_SCL 22
#else
  #define I2C_SDA 8
  #define I2C_SCL 9
#endif

Adafruit_BMP085 bmp;

const std::string TEMPERATURE_CODE = "t";
const std::string PRESSURE_CODE = "p";

OperationResult BMPSensor::setupSensor() {
  Serial.println("Setup BMP");

  // Initialize I2C communication with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("Setup BMP 2");
  
  // Initialize BMP085 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    return OperationResult::NOT_FOUND;
  }
  
  Serial.println("BMP085 initialization successful!");
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> BMPSensor::getSensorData() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  //float altitude = bmp.readAltitude();

  int convertedTemp = temperature*100;
  int convertedPressure = pressure*10;

  std::map<std::string, std::string> resultMap;
  resultMap.insert(std::make_pair(TEMPERATURE_CODE, std::to_string(convertedTemp)));
  resultMap.insert(std::make_pair(PRESSURE_CODE, std::to_string(convertedPressure)));

  return resultMap;
}