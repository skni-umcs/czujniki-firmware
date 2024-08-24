#include <utils/operationResult.h>
#include "jsonTransmit.h"
#include <iostream>

OperationResult JsonTransmit::notifySubscribers(std::string message) {
    return OperationResult::SUCCESS;
}

OperationResult JsonTransmit::addSubscriber(std::weak_ptr<Communication> communication) {
    return OperationResult::SUCCESS;
}