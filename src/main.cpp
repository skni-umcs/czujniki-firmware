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
#include "sensors/subtypes/humidity_temperature_sensor.h"
#include "sensors/sensorFacade.h"
#include <sensors/subtypes/bme_280_sensor.h>
#include <iostream>
#include "time/timerUpdate.h"
#include "time/timer.h"

uint32_t delayMS = 1000;

void setup() {
  Serial.begin(9600);
  SensorFacade facade = SensorFacade();
  std::unique_ptr<Sensor> h = std::unique_ptr<BME280Sensor>(new BME280Sensor());
  h->setupSensor(&delayMS);
  facade.addSensor(std::move(h));
}

void loop() {
  //TimerUpdate::notifySubscribers();
}