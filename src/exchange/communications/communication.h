#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <utils/operationResult.h>

class Communication
{
    public:
        virtual OperationResult getNotified(std::string message) = 0;
        virtual OperationResult transmit(std::string message) = 0;
};

#endif