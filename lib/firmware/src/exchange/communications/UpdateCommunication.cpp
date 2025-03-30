#include <string>
#include "UpdateCommunication.h"
#include <memory>
#include <utils/otherUtils.h>
#include "utils/addressHandler.h"
#include <Arduino_ESP32_OTA.h>

std::shared_ptr<UpdateCommunication> UpdateCommunication::create() {
    auto s = new UpdateCommunication();
    return std::shared_ptr<UpdateCommunication>{s};
}

static char const OTA_FILE_LOCATION[] = "https://raw.githubusercontent.com/Osariusz/ota_test2/main/firmware.ota";

OperationResult UpdateCommunication::getNotified(std::shared_ptr<Message> message) {
    if(message->getIsPacketCorrect() && message->getContent() == "u") {
        Serial.printf("received: %s", message->getContent());
        update();
    }
    return OperationResult::SUCCESS;
}
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

OperationResult UpdateCommunication::update() {
    const char* firmware_url = "http://192.168.1.18:8000/firmware.bin";

    Serial.println("Starting OTA update from local server...");

    WiFiClient client;
    HTTPClient http;

    http.begin(client, firmware_url);
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