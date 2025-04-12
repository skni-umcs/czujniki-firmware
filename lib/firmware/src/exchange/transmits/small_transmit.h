#ifndef SMALLTRANSMIT_H
#define SMALLTRANSMIT_H

#include <string>
#include <vector>
#include <exchange/transmits/transmit.h>

class SmallCommunication;

class SmallTransmit : virtual public Transmit<SmallCommunication>
{
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
};

#endif