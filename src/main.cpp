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
#include <utils/packetUtils.h>
#include "exchange/transmits/DEBUG_timeTransmit.h"
#include "exchange/transmits/wifiTransmit.h"
#include "sensors/subtypes/cpu_sensor.h"
#include "sensors/subtypes/noise_sensor.h"
#include <vector>
#include "exchange/communications/UpdateCommunication.h"

#if defined(esp32firebeetle)
using TRANSMIT_TYPE = DEBUG_timeTransmit;
#else
using TRANSMIT_TYPE = LoraTransmit;
#endif


void setup() {
  Serial.begin(9600);
  delay(1000);

  AddressHandler::getInstance().get()->initializeAddress();
  auto transmit = TRANSMIT_TYPE::create();

  auto wifiTransmit = WifiTransmit::create();

  delay(6000);
  
  auto serviceCommunication = ServiceCommunication::create();
  serviceCommunication.get()->subscribe(transmit);
  serviceCommunication.get()->sendResetReason();
  delay(1000);
  serviceCommunication.get()->askForTime();

  std::shared_ptr<SensorFacade> facade = SensorFacade::create(transmit);

  //auto passthroughCommunication = PassthroughCommunication::create();
  //passthroughCommunication.get()->subscribe(transmit);

  Serial.println("update com");

  auto updateCommunication = UpdateCommunication::create();
  Serial.println("mid");
  updateCommunication->subscribe(wifiTransmit);
  Serial.println("after sub");

}

void loop() {
  //disabled due to using sleep
}