#include <string>
#include "sensor_communication.h"
#include <memory>

std::shared_ptr<SensorCommunication> SensorCommunication::create() {
    auto s = new SensorCommunication();
    return std::shared_ptr<SensorCommunication>{s};
}

OperationResult SensorCommunication::getNotified(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}