#include <utils/operationResult.h>
#include "wifiTransmit.h"
#include <iostream>

OperationResult WifiTransmit::send(std::string message, moduleAddress destinationNode) {
    std::cout << "wifi sending\n" << message << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::receive(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}
