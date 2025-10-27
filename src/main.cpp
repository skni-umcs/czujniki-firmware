// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <exchange/communications/config_communication.h>
#include <exchange/communications/passthrough_communication.h>
#include <exchange/communications/service_communication.h>
#include <exchange/transmits/lora_transmit.h>
#include <message/message_decode_utils.h>

#include <vector>

#include "exchange/transmits/DEBUG_time_transmit.h"
#include "exchange/transmits/wifi_transmit.h"
#include "sensors/sensor_facade.h"
#include "time/time_slot_manager.h"
#include "time/timer.h"
#include "utils/address_handler.h"
#include "utils/configuration_facade.h"
#include "utils/logger.h"

#if defined(production)
using TRANSMIT_TYPE = LoraTransmit;
#else
using TRANSMIT_TYPE = DEBUG_timeTransmit;
#endif

void setup() {
  Serial.begin(9600);
  delay(1000);

  // auto wifiTransmit = WifiTransmit::create();
  // Logger::setWifi(wifiTransmit);

  delay(5000);
  AddressHandler::getInstance().get()->initializeAddress();

  // Utwórz TimeSlotManager z adresem modułu
  moduleAddress myAddress = AddressHandler::getInstance()->readAddress();
  auto timeSlotManager = std::make_shared<TimeSlotManager>(myAddress);
  Logger::logf("TimeSlotManager created for address %d - assigned slot %d\n",
               myAddress, timeSlotManager->getMySlotNumber());

  auto transmit = TRANSMIT_TYPE::create();  // wifiTransmit);
  delay(1000);

// Ustaw TimeSlotManager w module transmisji (tylko dla LoRa w trybie
// produkcyjnym)
#if defined(production)
  transmit->setTimeSlotManager(timeSlotManager);
#endif

  auto sensorCommunication = SensorCommunication::create();

  auto serviceCommunication = ServiceCommunication::create();
  serviceCommunication.get()->subscribe(transmit);
  serviceCommunication.get()->sendResetReason();

  // Ustaw TimeSlotManager w ServiceCommunication aby otrzymywał powiadomienia o
  // sync czasu
  serviceCommunication.get()->setTimeSlotManager(timeSlotManager);

  std::shared_ptr<SensorFacade> facade =
      SensorFacade::create(transmit, sensorCommunication, serviceCommunication);

  auto passthroughCommunication = PassthroughCommunication::create();
  passthroughCommunication.get()->subscribe(transmit);
  passthroughCommunication.get()->setTimeSlotManager(timeSlotManager);

  // auto updateCommunication = UpdateCommunication::create();
  //  updateCommunication->subscribe(wifiTransmit);

  auto configurationFacade =
      std::shared_ptr<ConfigurationFacade>(new ConfigurationFacade());
  configurationFacade->init();
#if defined(production)
  configurationFacade->plugLoraTransmit(transmit);
#endif
  configurationFacade->plugPassthroughCommunication(passthroughCommunication);
  configurationFacade->plugSensorFacade(facade);
  configurationFacade->plugServiceCommunication(serviceCommunication);

  auto configCommunication = ConfigCommunication::create(configurationFacade);
  configCommunication->subscribe(transmit);
}

void loop() {
  // disabled due to using sleep
}