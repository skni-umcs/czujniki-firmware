#include <string>
#include "UpdateCommunication.h"
#include <memory>
#include <utils/otherUtils.h>
#include "utils/addressHandler.h"
#include <Arduino_ESP32_OTA.h>
#include <root_ca.h>

std::shared_ptr<UpdateCommunication> UpdateCommunication::create() {
    auto s = new UpdateCommunication();
    return std::shared_ptr<UpdateCommunication>{s};
}

static char const OTA_FILE_LOCATION[] = "https://raw.githubusercontent.com/Osariusz/ota_test/main/firmware.ota";

OperationResult UpdateCommunication::getNotified(std::shared_ptr<Message> message) {
    if(message->getIsPacketCorrect() && message->getContent() == "u") {
        Serial.printf("received: %s", message->getContent());
        update();
    }
    return OperationResult::SUCCESS;
}

OperationResult UpdateCommunication::update() {
    //TODO: add check if transmit medium is up
    Serial.println("try ota");
    Arduino_ESP32_OTA ota;
    Arduino_ESP32_OTA::Error ota_err = Arduino_ESP32_OTA::Error::None;
    ota.setCACert(root_ca);
    Serial.println("certified.");
    Serial.println("Initializing OTA storage");
    if ((ota_err = ota.begin()) != Arduino_ESP32_OTA::Error::None)
    {
        Serial.print  ("Arduino_ESP_OTA::begin() failed with error code ");
        Serial.println((int)ota_err);
        return OperationResult::ERROR;
    }
    int const ota_download = ota.download(OTA_FILE_LOCATION);
    Serial.println(" bytes stored:");
    Serial.print(ota_download);
   

    Serial.println("Verify update integrity and apply ...");
    if ((ota_err = ota.update()) != Arduino_ESP32_OTA::Error::None)
    {
      Serial.print  ("ota.update() failed with error code ");
      Serial.println((int)ota_err);
      return OperationResult::ERROR;
    }
    ota.reset();

    return OperationResult::SUCCESS;
}

