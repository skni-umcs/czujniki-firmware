#include <string>
#include "sensor_communication.h"
#include <memory>

int sensorTransmits = 0;

std::shared_ptr<SensorCommunication> SensorCommunication::create() {
    auto s = new SensorCommunication();
    return std::shared_ptr<SensorCommunication>{s};
}

OperationResult SensorCommunication::transmit(std::string message, moduleAddress destinationNode) {
    sensorTransmits++;
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
        destination->send("Real sensor transmits: "+std::to_string(sensorTransmits), 0);
    }
    return OperationResult::SUCCESS;
}

OperationResult SensorCommunication::transmit(std::shared_ptr<Message> message) {
    sensorTransmits++;
    for(auto const& destination : transmitTo) {
        destination->send(message);
        destination->send("Real sensor transmits: "+std::to_string(sensorTransmits), 0);
    }
    return OperationResult::SUCCESS;
}

OperationResult SensorCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}