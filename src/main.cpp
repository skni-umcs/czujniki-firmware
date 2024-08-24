// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "sensors/subtypes/humidity_sensor.h"
#include "sensors/sensorFacade.h"

uint32_t delayMS;
SensorFacade facade;

void setup() {
  Serial.begin(9600);
  facade = SensorFacade();
  std::unique_ptr<Sensor> h = std::unique_ptr<HumiditySensor>(new HumiditySensor());
  h->setupSensor(&delayMS);
  facade.addSensor(std::move(h));
}

void loop() {
  delay(delayMS);
  facade.sendAllSensors();
}