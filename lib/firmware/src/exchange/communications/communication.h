#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <utils/operation_result.h>
#include <memory>
#include "utils/storage_types.h"
#include <iostream>

template <class TTransmit, typename Derived>
class Communication : public std::enable_shared_from_this<Derived>
{
    protected:
        std::vector<std::shared_ptr<TTransmit>> transmitTo;
    //TODO: Disable default constructor for all communications to prevent their creation outside of the create() function
    public:
        OperationResult subscribe(std::shared_ptr<TTransmit> transmit) {
            auto thisPtr = this->shared_from_this();
            transmit->addSubscriber(thisPtr);
            transmitTo.push_back(transmit);
            return OperationResult::SUCCESS;
        }
        OperationResult unsubscribe(std::shared_ptr<TTransmit> transmit) {
            auto index = std::find(transmitTo.begin(), transmitTo.end(), transmit);
            if (index != transmitTo.end()) {
                transmitTo.erase(index);
                return OperationResult::SUCCESS;
            }
            return OperationResult::NOT_FOUND;
        }
        virtual OperationResult getNotified(std::shared_ptr<Message> message) = 0;
        virtual OperationResult transmit(std::string message, moduleAddress destinationNode) {
            for(auto const& destination : transmitTo) {
                destination->send(message, destinationNode);
            }
            return OperationResult::SUCCESS;
        }
        virtual OperationResult transmit(std::shared_ptr<Message> message) {
            for(auto const& destination : transmitTo) {
                destination->send(message);
            }
            return OperationResult::SUCCESS;
        }
        const std::vector<std::shared_ptr<TTransmit>>& getTransmitTo() {
            return transmitTo;
        }
};

#endif