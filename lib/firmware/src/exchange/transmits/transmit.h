#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include <memory>
#include "utils/storage_types.h"

class Transmit
{
    std::vector<std::shared_ptr<Communication>> subscribers;

    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
        OperationResult notifySubscribers(std::shared_ptr<Message> message);
        OperationResult addSubscriber(std::shared_ptr<Communication> communication);
};

#endif