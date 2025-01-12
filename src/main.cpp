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
#include <sensors/subtypes/test_sensor.h>
#include <iostream>
#include "time/timerUpdate.h"
#include "time/timer.h"
#include <exchange/transmits/loraTransmit.h>
#include <exchange/communications/serviceCommunication.h>
#include <exchange/communications/passthroughCommunication.h>
#include <Preferences.h>
#include "utils/addressHandler.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

uint32_t delayMS = 1000;

#if defined(esp32firebeetle) || defined(mini_test)
#define SENSOR_TYPE TestSensor
#else
#define SENSOR_TYPE HumidityTemperatureSensor
#endif

void setup() {
  Serial.begin(9600);
  delay(1000);

  AddressHandler::getInstance().get()->initializeAddress();
  auto transmit = LoraTransmit::create();
  
  auto serviceCommunication = ServiceCommunication::create();
  serviceCommunication.get()->subscribe(transmit);
  serviceCommunication.get()->sendResetReason();

  std::shared_ptr<SensorFacade> facade = SensorFacade::create(transmit);
  std::unique_ptr<Sensor> h = std::unique_ptr<SENSOR_TYPE>(new SENSOR_TYPE());
  h->setupSensor(&delayMS);
  facade->addSensor(std::move(h));

  auto passthroughCommunication = PassthroughCommunication::create();
  passthroughCommunication.get()->subscribe(transmit);
}

void loop() {
  //disabled due to using sleep
}