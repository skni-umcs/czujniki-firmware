#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <string>
#include <memory>
#include "message/message.h"

template <class TTransmit, typename Derived>
class Communication;

template <class TCommunication>
class Transmit
{
    std::vector<std::shared_ptr<TCommunication>> subscribers;

    public:
        virtual OperationResult send(std::shared_ptr<Message> message) = 0;
        virtual OperationResult receive(std::shared_ptr<Message> message) = 0;
        OperationResult notifySubscribers(std::shared_ptr<Message> message) {
            for(auto subscriber : subscribers) {
                subscriber->getNotified(message);
            }
            return OperationResult::SUCCESS;
        }
        OperationResult addSubscriber(std::shared_ptr<TCommunication> communication) {
            subscribers.push_back(communication);
            return OperationResult::SUCCESS;
        }
        virtual TransmitType type() const = 0;
};

#endif