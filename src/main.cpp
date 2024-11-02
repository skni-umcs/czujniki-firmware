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
#include "sensors/subtypes/test_sensor.h"
#include "sensors/sensorFacade.h"
#include <sensors/subtypes/bme_280_sensor.h>
#include <iostream>
#include "time/timerUpdate.h"
#include "time/timer.h"

uint32_t delayMS = 1000;
SensorFacade facade;
std::shared_ptr<Timer> timer = std::shared_ptr<Timer>(new Timer());

void setup() {
  Serial.begin(9600);
  facade = SensorFacade();
  std::unique_ptr<Sensor> h = std::unique_ptr<BME280Sensor>(new BME280Sensor());
  std::unique_ptr<Sensor> t = std::unique_ptr<TestSensor>(new TestSensor());
  h->setupSensor(&delayMS);
  t->setupSensor(&delayMS);
  facade.addSensor(std::move(h));
  facade.addSensor(std::move(t));
  TimerUpdate::addSubscriber(timer);
  timer.get()->updateTime(10,10);
}

void loop() {
  //TimerUpdate::notifySubscribers();
}