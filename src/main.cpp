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
#include "sensors/sensor_facade.h"
#include <sensors/subtypes/test_sensor.h>
#include <iostream>
#include "time/timer_update.h"
#include "time/timer.h"
#include <exchange/transmits/lora_transmit.h>
#include <exchange/communications/service_communication.h>
#include <exchange/communications/passthrough_communication.h>
#include <Preferences.h>
#include "utils/address_handler.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <message/message_decode_utils.h>
#include "exchange/transmits/DEBUG_time_transmit.h"
#include "exchange/transmits/wifi_transmit.h"
#include "sensors/subtypes/cpu_sensor.h"
#include "sensors/subtypes/noise_sensor.h"
#include <vector>
#include "exchange/communications/update_communication.h"

#if defined(esp32firebeetle)
using TRANSMIT_TYPE = DEBUG_timeTransmit;
#else
using TRANSMIT_TYPE = LoraTransmit;
#endif


void setup() {
  Serial.begin(9600);
  delay(1000);

  AddressHandler::getInstance().get()->initializeAddress();
  

  auto wifiTransmit = WifiTransmit::create();
  auto transmit = TRANSMIT_TYPE::create(wifiTransmit);
  delay(6000);
  
  auto serviceCommunication = ServiceCommunication::create();
  serviceCommunication.get()->subscribe(transmit);
  serviceCommunication.get()->sendResetReason();
  delay(1000);
  serviceCommunication.get()->askForTime();

  std::shared_ptr<SensorFacade> facade = SensorFacade::create(transmit);

  auto passthroughCommunication = PassthroughCommunication::create();
  passthroughCommunication.get()->subscribe(transmit);

  auto updateCommunication = UpdateCommunication::create();
  updateCommunication->subscribe(wifiTransmit);

}

void loop() {
  //disabled due to using sleep
}