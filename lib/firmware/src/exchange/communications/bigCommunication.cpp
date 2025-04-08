#include "bigCommunication.h"

//TODO: rewrite to template and unify with jsoncommunication but with different transmit types
OperationResult BigCommunication::subscribe(std::shared_ptr<BigTransmit> transmit) {
    auto thisPtr = shared_from_this();
    transmit->addSubscriber(thisPtr);
    transmitTo.push_back(transmit);
    return OperationResult::SUCCESS;
}

OperationResult BigCommunication::unsubscribe(std::shared_ptr<BigTransmit> transmit) {
    auto index = std::find(transmitTo.begin(), transmitTo.end(), transmit);
    if (index != transmitTo.end()) {
        transmitTo.erase(index);
        return OperationResult::SUCCESS;
    }
    return OperationResult::NOT_FOUND;
}

OperationResult BigCommunication::transmit(std::string message, moduleAddress destinationNode) {
    for(auto const& destination : transmitTo) {
        destination->send(message, destinationNode);
    }
    return OperationResult::SUCCESS;
}

OperationResult BigCommunication::transmit(std::shared_ptr<Message> message) {
    for(auto const& destination : transmitTo) {
        destination->send(message);
    }
    return OperationResult::SUCCESS;
}

const std::vector<std::shared_ptr<BigTransmit>>& BigCommunication::getTransmitTo() {
    return transmitTo;
}