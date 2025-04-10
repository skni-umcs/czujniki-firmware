#include <string>
#include <algorithm> 
#include "small_communication.h"
#include <iostream>
#include <memory>
#include <HardwareSerial.h>

std::shared_ptr<SmallCommunication> SmallCommunication::create() {
    return std::shared_ptr<SmallCommunication>{new SmallCommunication()};
}

//TODO: every communication has its own notify mechanism, this should be pure virtual
OperationResult SmallCommunication::getNotified(std::shared_ptr<Message> message) {
    if(message->getIsPacketCorrect()) {
        //Serial.printf("im notified of %s from %i to %i\n", message.get()->getContent().c_str(), message.get()->getOriginalSender(), message.get()->getDestination());
    }
    return OperationResult::SUCCESS;
}

OperationResult SmallCommunication::transmit(std::string message, moduleAddress destinationNode) {
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
    }
    return OperationResult::SUCCESS;
}

OperationResult SmallCommunication::transmit(std::shared_ptr<Message> message) {
    for(auto const& destination : transmitTo) {
        destination->send(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult SmallCommunication::subscribe(std::shared_ptr<SmallTransmit> transmit) {
    auto thisPtr = shared_from_this();
    transmit->addSubscriber(thisPtr);
    transmitTo.push_back(transmit);
    return OperationResult::SUCCESS;
}

OperationResult SmallCommunication::unsubscribe(std::shared_ptr<SmallTransmit> transmit) {
    auto index = std::find(transmitTo.begin(), transmitTo.end(), transmit);
    if (index != transmitTo.end()) {
        transmitTo.erase(index);
        return OperationResult::SUCCESS;
    }
    return OperationResult::NOT_FOUND;
}

const std::vector<std::shared_ptr<SmallTransmit>>& SmallCommunication::getTransmitTo() {
    return transmitTo;
}