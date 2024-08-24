#include <string>
#include <algorithm> 
#include "sensorCommunication.h"
#include <iostream>
#include <memory>

OperationResult JsonCommunication::getNotified(std::string message) {
    std::cout << "im notified of" << message;
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::transmit(std::string message) {
    std::cout << "transmitting message: " << message;
    for(auto const& destination : transmitTo) {
        destination.get()->send(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::subscribe(std::unique_ptr<JsonTransmit> transmit) {
    transmitTo.push_back(std::move(transmit));
    return OperationResult::SUCCESS;
}

OperationResult JsonCommunication::unsubscribe(std::unique_ptr<JsonTransmit> transmit) {
    auto index = std::find(transmitTo.begin(), transmitTo.end(), transmit);
    if (index != transmitTo.end()) {
        transmitTo.erase(index);
        return OperationResult::SUCCESS;
    }
    return OperationResult::NOT_FOUND;
}