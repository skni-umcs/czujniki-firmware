#ifndef BIGTRANSMIT_H
#define BIGTRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "utils/storageTypes.h"

class BigTransmit : public Transmit
{
    std::vector<std::shared_ptr<Communication>> subscribers;
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
        OperationResult notifySubscribers(std::shared_ptr<Message> message) override;
        OperationResult addSubscriber(std::shared_ptr<Communication> communication) override;
};

#endif