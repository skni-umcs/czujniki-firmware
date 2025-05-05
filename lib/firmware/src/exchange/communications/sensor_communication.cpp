#include <string>
#include "sensor_communication.h"
#include <memory>

std::shared_ptr<SensorCommunication> SensorCommunication::create() {
    auto s = new SensorCommunication();
    return std::shared_ptr<SensorCommunication>{s};
}

OperationResult SensorCommunication::transmit(std::string message, moduleAddress destinationNode) {
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
    }
    return OperationResult::SUCCESS;
}

OperationResult SensorCommunication::transmit(std::shared_ptr<Message> message) {
    for(auto const& destination : transmitTo) {
        destination->send(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult SensorCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}