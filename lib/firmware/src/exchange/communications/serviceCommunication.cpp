#include <string>
#include "serviceCommunication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>

std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto s = new ServiceCommunication();
    return std::shared_ptr<ServiceCommunication>{s};
}

void ServiceCommunication::sendResetReason() {
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    std::string reset_reasons[] = {
        "r",
        std::to_string(rtc_get_reset_reason(0)), 
        std::to_string(rtc_get_reset_reason(1))
    };
    for (const auto& reason : reset_reasons) {
        messages.add(reason);
    }

    serializeJson(doc, serializedJson);
    this->transmit(serializedJson, SERVER_ADDRESS);
}