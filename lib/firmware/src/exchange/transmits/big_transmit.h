#ifndef BIGTRANSMIT_H
#define BIGTRANSMIT_H

#include <string>
#include <exchange/transmits/transmit.h>
#include "utils/message.h"

class BigCommunication;

class BigTransmit : virtual public Transmit<BigCommunication>
{
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
};

#endif