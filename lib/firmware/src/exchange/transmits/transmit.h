#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include <memory>
#include "utils/storageTypes.h"

class Transmit
{
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult receive(Message message) = 0;
        virtual OperationResult notifySubscribers(Message message) = 0;
        virtual OperationResult addSubscriber(std::shared_ptr<Communication> communication) = 0;
};

#endif