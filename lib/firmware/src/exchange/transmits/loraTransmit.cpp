#include <utils/operationResult.h>
#include "loraTransmit.h"
#include <iostream>

OperationResult LoraTransmit::send(std::string message) {
    std::cout << "lora sending\n" << message << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult LoraTransmit::receive(std::string message) {
    return OperationResult::SUCCESS;
}
