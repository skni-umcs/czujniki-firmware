#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <utils/operationResult.h>
#include <memory>
#include "types/message.h"

class Communication : public std::enable_shared_from_this<Communication>
{
    //TODO: Disable default constructor for all communications to prevent their creation outside of the create() function
    public:
        virtual OperationResult getNotified(Message message) = 0;
        virtual OperationResult transmit(Message message) = 0;
};

#endif