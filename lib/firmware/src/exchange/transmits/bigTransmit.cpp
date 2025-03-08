#include <utils/operationResult.h>
#include "bigTransmit.h"
#include <iostream>

OperationResult BigTransmit::notifySubscribers(std::shared_ptr<Message> message) {
    for(auto subscriber : subscribers) {
        subscriber->getNotified(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult BigTransmit::addSubscriber(std::shared_ptr<Communication> communication) {
    subscribers.push_back(communication);
    return OperationResult::SUCCESS;
}