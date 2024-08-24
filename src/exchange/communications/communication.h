#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>

class Communication
{
    public:
        virtual std::string getNotified(std::string message) = 0;
};

#endif