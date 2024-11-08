#include <utils/operationResult.h>
#include "testTransmit.h"
#include <iostream>

OperationResult TestTransmit::send(std::string message) {
    std::cout << "lora sending\n" << message << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult TestTransmit::receive(std::string message) {
    return OperationResult::SUCCESS;
}
