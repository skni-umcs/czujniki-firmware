#include "transmit.h"
#include <utils/operation_result.h>

OperationResult Transmit::notifySubscribers(std::shared_ptr<Message> message) {
    for(auto subscriber : subscribers) {
        subscriber->getNotified(message);
    }
    return OperationResult::SUCCESS;
}

OperationResult Transmit::addSubscriber(std::shared_ptr<Communication> communication) {
    subscribers.push_back(communication);
    return OperationResult::SUCCESS;
}