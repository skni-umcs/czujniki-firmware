#ifndef JSONTRANSMIT_H
#define JSONTRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"

class JsonTransmit : public Transmit
{
    public:
        virtual OperationResult send(std::string message) = 0;
        virtual OperationResult receive(std::string message) = 0;
        virtual OperationResult notifySubscribers(std::string message) = 0;
        virtual OperationResult addSubscriber(std::unique_ptr<Communication> communication) = 0;
};

#endif