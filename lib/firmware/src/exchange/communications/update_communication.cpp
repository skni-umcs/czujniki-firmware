#include <string>
#include "update_communication.h"
#include <memory>
#include <utils/other_utils.h>
#include "utils/address_handler.h"
#include <Arduino_ESP32_OTA.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "exchange/communications/big_communication.h"
#include "exchange/communications/small_communication.h"
#include "exchange/transmits/big_transmit.h"
#include <utils/logger.h>

std::shared_ptr<UpdateCommunication> UpdateCommunication::create() {
    auto s = new UpdateCommunication();
    return std::shared_ptr<UpdateCommunication>{s};
}

static char const OTA_FILE_LOCATION[] = "http://10.50.50.123:8000/firmware.bin";

OperationResult UpdateCommunication::getNotified(std::shared_ptr<Message> message) {
    if(message->getIsPacketCorrect() && message->getContent() == "u") {
        Logger::logf("received: %s", message->getContent());
        update();
    }
    return OperationResult::SUCCESS;
}

OperationResult UpdateCommunication::update() {
    Logger::log("Starting OTA update from local server...");

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
                Logger::logf("Written only %d/%d bytes. Abort.\n", written, contentLength);
                http.end(); // Close connection before returning
                return OperationResult::ERROR;
            }

            if (Update.end()) {
                Logger::log("OTA update finished!");
                if (Update.isFinished()) {
                    Logger::log("Update successfully completed. Rebooting...");
                    http.end(); // Close HTTP connection before restarting
                    ESP.restart();
                } else {
                    Logger::log("Update not finished. Something went wrong.");
                    http.end(); // Close connection on error
                    return OperationResult::ERROR;
                }
            } else {
                Logger::logf("Error Occurred. Error #: %d\n", Update.getError());
                http.end(); // Close connection on error
                return OperationResult::ERROR;
            }
        } else {
            Logger::log("Not enough space to begin OTA.");
            http.end(); // Close connection on error
            return OperationResult::ERROR;
        }
    } else {
        Logger::logf("Failed to download firmware. HTTP error: %d\n", httpCode);
        http.end(); // Close connection on error
        return OperationResult::ERROR;
    }

    // This line is unreachable if update succeeds, but included for safety
    http.end();
    return OperationResult::SUCCESS;
}
