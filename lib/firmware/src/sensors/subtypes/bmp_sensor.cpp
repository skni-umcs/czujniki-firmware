#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "bmp_sensor.h"
#include <sstream>
#include <utility>

// Define I2C pins
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_BMP085 bmp;

const std::string TEMPERATURE_CODE = "t";
const std::string PRESSURE_CODE = "p";

void BMPSensor::setupSensor(uint32_t* delayMS) {
  Serial.begin(9600);
  
  // Initialize I2C communication with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize BMP085 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    return;
  }
  
  Serial.println("BMP085 initialization successful!");
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