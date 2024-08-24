#include <utils/operationResult.h>
#include "jsonTransmit.h"
#include <iostream>

OperationResult JsonTransmit::send(std::string message) {
    std::cout << "jsonTransmit sending" << message;
    return OperationResult::SUCCESS;
}