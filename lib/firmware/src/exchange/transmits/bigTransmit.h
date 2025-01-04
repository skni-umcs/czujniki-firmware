#ifndef BIGTRANSMIT_H
#define BIGTRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "utils/storageTypes.h"

class BigTransmit : public Transmit
{
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult receive(std::string message) = 0;
        virtual OperationResult notifySubscribers(std::string message) = 0;
        virtual OperationResult addSubscriber(std::shared_ptr<Communication> communication) = 0;
};

#endif