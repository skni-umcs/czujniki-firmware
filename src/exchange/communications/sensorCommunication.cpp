#include <string>
#include "sensorCommunication.h"
#include <iostream>
#include <memory>

std::string SensorCommunication::getNotified(std::string message) {
    std::cout << "im notified";
    return "was notified";
}

void SensorCommunication::subscribe(std::unique_ptr<JsonTransmit> transmit) {
    transmitTo.push_back(std::move(transmit));
}