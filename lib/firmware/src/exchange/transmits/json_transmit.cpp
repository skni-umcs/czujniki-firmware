#include <utils/operation_result.h>
#include "json_transmit.h"
#include <iostream>

OperationResult JsonTransmit::notifySubscribers(std::shared_ptr<Message> message) {
    for(auto subscriber : subscribers) {
        subscriber->getNotified(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult JsonTransmit::addSubscriber(std::shared_ptr<Communication> communication) {
    subscribers.push_back(communication);
    return OperationResult::SUCCESS;
}