#ifndef BIGTRANSMIT_H
#define BIGTRANSMIT_H

#include <string>
#include <exchange/transmits/transmit.h>
#include "message/message.h"

class BigCommunication;

class BigTransmit : virtual public Transmit<BigCommunication>
{
    public:
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
};

#endif