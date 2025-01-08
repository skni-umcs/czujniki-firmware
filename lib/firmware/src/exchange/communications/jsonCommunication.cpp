#include <string>
#include <algorithm> 
#include "jsonCommunication.h"
#include <iostream>
#include <memory>
#include <HardwareSerial.h>

std::shared_ptr<JsonCommunication> JsonCommunication::create() {
    return std::shared_ptr<JsonCommunication>{new JsonCommunication()};
}

OperationResult JsonCommunication::getNotified(std::shared_ptr<Message> message) {
    Serial.printf("im notified of %s from %i to %i\n", message.get()->getContent().c_str(), message.get()->getOriginalSender(), message.get()->getDestination());
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::transmit(std::string message, moduleAddress destinationNode) {
    Serial.printf("generic transmitting message: %s\n", message.c_str());
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
    }
    Serial.println("finished generic transmit message");
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::subscribe(std::shared_ptr<JsonTransmit> transmit) {
    auto thisPtr = shared_from_this();
    transmit->addSubscriber(thisPtr);
    transmitTo.push_back(transmit);
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::unsubscribe(std::shared_ptr<JsonTransmit> transmit) {
    auto index = std::find(transmitTo.begin(), transmitTo.end(), transmit);
    if (index != transmitTo.end()) {
        transmitTo.erase(index);
        return OperationResult::SUCCESS;
    }
    return OperationResult::NOT_FOUND;
}

const std::vector<std::shared_ptr<JsonTransmit>>& JsonCommunication::getTransmitTo() {
    return transmitTo;
}