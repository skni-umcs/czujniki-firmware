#ifndef JSONTRANSMIT_H
#define JSONTRANSMIT_H

#include <string>
#include <vector>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "utils/storage_types.h"

class JsonTransmit : virtual public Transmit
{
    public:
        virtual OperationResult send(std::string message, moduleAddress destinationNode) = 0;
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
};

#endif