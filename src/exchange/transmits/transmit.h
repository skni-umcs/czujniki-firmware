#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include <memory>

class Transmit
{
    public:
        virtual void send(std::string message) = 0;
        virtual void receive(std::string message) = 0;
        virtual void notifySubscribers(std::string message) = 0;
        virtual std::string addSubscriber(std::unique_ptr<Communication> communication) = 0;
};

#endif