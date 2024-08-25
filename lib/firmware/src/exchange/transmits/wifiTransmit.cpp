#include <utils/operationResult.h>
#include "wifiTransmit.h"
#include <iostream>

OperationResult WifiTransmit::send(std::string message) {
    std::cout << "wifi sending\n" << message << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::receive(std::string message) {
    return OperationResult::SUCCESS;
}
