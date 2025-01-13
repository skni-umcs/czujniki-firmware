#include <string>
#include "serviceCommunication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>
#include "utils/packetUtils.h"

std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto s = new ServiceCommunication();
    return std::shared_ptr<ServiceCommunication>{s};
}

OperationResult ServiceCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Serial.printf("service is notified of invalid packet %s\n", message.get()->getPacket().c_str());
    }
    return OperationResult::SUCCESS;
}

void ServiceCommunication::sendResetReason() {
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    int reset_reasons[] = {
        rtc_get_reset_reason(0), 
        rtc_get_reset_reason(1)
    };
    for (const auto& reason : reset_reasons) {
        messages.add(reason);
    }
    serializeJson(doc, serializedJson);
    PacketMessage packetMessage = PacketMessage(TransmissionCode::RESET, serializedJson);

    this->transmit(packetMessage.getJson(), SERVER_ADDRESS);
}