#include <string>
#include "update_communication.h"
#include <memory>
#include <utils/other_utils.h>
#include "utils/address_handler.h"
#include <Arduino_ESP32_OTA.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

std::shared_ptr<UpdateCommunication> UpdateCommunication::create() {
    auto s = new UpdateCommunication();
    return std::shared_ptr<UpdateCommunication>{s};
}

static char const OTA_FILE_LOCATION[] = "http://192.168.1.18:8000/firmware.bin";

OperationResult UpdateCommunication::getNotified(std::shared_ptr<Message> message) {
    if(message->getIsPacketCorrect() && message->getContent() == "u") {
        Serial.printf("received: %s", message->getContent());
        update();
    }
    return OperationResult::SUCCESS;
}

OperationResult UpdateCommunication::update() {
    Serial.println("Starting OTA update from local server...");

    WiFiClient client;
    HTTPClient http;

    http.begin(client, OTA_FILE_LOCATION);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        bool canBegin = Update.begin(contentLength);

        if (canBegin) {
            WiFiClient* stream = http.getStreamPtr();
            size_t written = Update.writeStream(*stream);

            if (written != contentLength) {
                Serial.printf("Written only %d/%d bytes. Abort.\n", written, contentLength);
                http.end(); // Close connection before returning
                return OperationResult::ERROR;
            }

            if (Update.end()) {
                Serial.println("OTA update finished!");
                if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting...");
                    http.end(); // Close HTTP connection before restarting
                    ESP.restart();
                } else {
                    Serial.println("Update not finished. Something went wrong.");
                    http.end(); // Close connection on error
                    return OperationResult::ERROR;
                }
            } else {
                Serial.printf("Error Occurred. Error #: %d\n", Update.getError());
                http.end(); // Close connection on error
                return OperationResult::ERROR;
            }
        } else {
            Serial.println("Not enough space to begin OTA.");
            http.end(); // Close connection on error
            return OperationResult::ERROR;
        }
    } else {
        Serial.printf("Failed to download firmware. HTTP error: %d\n", httpCode);
        http.end(); // Close connection on error
        return OperationResult::ERROR;
    }

    // This line is unreachable if update succeeds, but included for safety
    http.end();
    return OperationResult::SUCCESS;
}