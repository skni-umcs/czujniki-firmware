#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "bmp_sensor.h"
#include <sstream>

// Define I2C pins
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_BMP085 bmp;

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

std::string BMPSensor::getSensorDataJson() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude();

  std::stringstream result;
  result << "temperature: " << temperature << " pressure: " << pressure << " altitude: " << altitude;
  return result.str();
}