#include <string>
#include <algorithm> 
#include "jsonCommunication.h"
#include <iostream>
#include <memory>
#include <HardwareSerial.h>

std::shared_ptr<JsonCommunication> JsonCommunication::create() {
    return std::shared_ptr<JsonCommunication>{new JsonCommunication()};
}

OperationResult JsonCommunication::getNotified(std::string message) {
    std::cout << "im notified of" << message << "\n";
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::transmit(std::string message, int destinationNode) {
    std::cout << "generic transmitting message: \n" << message << "\n";
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
    }
    std::cout << "finished generic transmit message\n";
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