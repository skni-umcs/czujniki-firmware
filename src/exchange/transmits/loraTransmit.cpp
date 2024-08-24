#include <utils/operationResult.h>
#include "loraTransmit.h"
#include <iostream>

OperationResult LoraTransmit::send(std::string message) {
    std::cout << "jsonTransmit sending" << message << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::receive(std::string message) {
    return OperationResult::SUCCESS;
}
