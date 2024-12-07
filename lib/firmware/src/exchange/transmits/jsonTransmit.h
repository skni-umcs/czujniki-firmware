#ifndef JSONTRANSMIT_H
#define JSONTRANSMIT_H

#include <string>
#include <vector>
#include <exchange/communications/communication.h>
#include "transmit.h"

class JsonTransmit : public Transmit
{
    std::vector<std::shared_ptr<Communication>> subscribers;

    public:
        virtual OperationResult send(std::string message, int destinationNode) = 0;
        virtual OperationResult receive(std::string message) = 0;
        OperationResult notifySubscribers(std::string message) override;
        OperationResult addSubscriber(std::shared_ptr<Communication> communication) override;
};

#endif