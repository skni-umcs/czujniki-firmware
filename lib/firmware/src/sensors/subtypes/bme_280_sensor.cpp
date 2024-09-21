#include "bme_280_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <string>
#include <sstream>
#include <iostream>

#define D5 9
#define D6 10
#define D7 13
#define D8 5

#define BME_SCK D8
#define BME_MISO D5
#define BME_MOSI D7
#define BME_CS D6

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void BME280Sensor::setupSensor(uint32_t* delayMS) {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();

}

std::string BME280Sensor::getSensorDataJson() {
    // Get temperature event and print its value.
    sensors_event_t event;
    std::stringstream result;
    result << "humidity: " << bme.readHumidity() << " temperature: " << bme.readTemperature() << " pressure: " << bme.readPressure() << " altitude: " << bme.readAltitude(SEALEVELPRESSURE_HPA);
    return result.str();
}