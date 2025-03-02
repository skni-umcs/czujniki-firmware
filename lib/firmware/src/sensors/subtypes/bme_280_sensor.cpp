#include "bme_280_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ArduinoJson.h>

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

OperationResult BME280Sensor::setupSensor() {
  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    return OperationResult::NOT_FOUND;
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  return OperationResult::SUCCESS;
}

struct BME280Output {
  float humidity;
  float temperature;
  float pressure;
  float altitude;

  BME280Output(float humidity, float temperature, float pressure, float altitude) {
    this->humidity = humidity;
    this->temperature = temperature;
    this->pressure = pressure;
    this->altitude = altitude;
  }

  void serialize(JsonDocument& doc) {
    JsonObject obj = doc.to<JsonObject>();
    obj["humidity"] = this->humidity;
    obj["temperature"] = this->temperature;
    obj["pressure"] = this->pressure;
    obj["altitude"] = this->altitude;
  }
};

std::map<std::string, std::string> BME280Sensor::getSensorData() {
    sensors_event_t event;
    BME280Output output = BME280Output(
      bme.readHumidity(),
      bme.readTemperature(),
      bme.readPressure(),
      bme.readAltitude(SEALEVELPRESSURE_HPA)
    );
    std::stringstream result;

    JsonDocument doc;
    output.serialize(doc);
    serializeJson(doc, result);
    std::map<std::string, std::string> resultMap; //TODO: fix sensor return
    return resultMap;
}