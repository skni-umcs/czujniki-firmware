// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include "sensors/sensor_facade.h"
#include "time/timer.h"
#include <exchange/transmits/lora_transmit.h>
#include <exchange/communications/service_communication.h>
#include <exchange/communications/passthrough_communication.h>
#include "utils/address_handler.h"
#include <message/message_decode_utils.h>
#include "exchange/transmits/DEBUG_time_transmit.h"
#include "exchange/transmits/wifi_transmit.h"
#include <vector>
#include "exchange/communications/update_communication.h"
#include "utils/logger.h"

#if defined(production)
using TRANSMIT_TYPE = LoraTransmit;
#else
using TRANSMIT_TYPE = DEBUG_timeTransmit;
#endif

void setup() {
  Serial.begin(9600);
  delay(1000);

  auto wifiTransmit = WifiTransmit::create();
  if(wifiTransmit) {
    Logger::setWifi(wifiTransmit);
  }

  delay(5000);
  AddressHandler::getInstance().get()->initializeAddress();


  auto transmit = TRANSMIT_TYPE::create(wifiTransmit); //TODO: wifi transmit not necessary here
  delay(1000);
  
  auto serviceCommunication = ServiceCommunication::create();
  serviceCommunication.get()->subscribe(transmit);
  serviceCommunication.get()->sendResetReason();
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