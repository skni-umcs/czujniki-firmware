#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <utils/operationResult.h>
#include <memory>
#include "utils/storageTypes.h"

class Communication : public std::enable_shared_from_this<Communication>
{
    //TODO: Disable default constructor for all communications to prevent their creation outside of the create() function
    public:
        virtual OperationResult getNotified(std::shared_ptr<Message> message) = 0;
        virtual OperationResult transmit(std::string message, moduleAddress destinationNode) = 0;
};

#endif