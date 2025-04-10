#ifndef SMALLCOMMUNICATION_H
#define SMALLCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/small_transmit.h>
#include "communication.h"
#include <memory>
#include "utils/storage_types.h"

class SmallCommunication : public Communication {
    
    protected:
        std::vector<std::shared_ptr<SmallTransmit>> transmitTo;
    public:
        static std::shared_ptr<SmallCommunication> create();
        virtual OperationResult getNotified(std::shared_ptr<Message> message) override;
        virtual OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        virtual OperationResult transmit(std::shared_ptr<Message> message) override;
        OperationResult subscribe(std::shared_ptr<SmallTransmit> transmit);
        OperationResult unsubscribe(std::shared_ptr<SmallTransmit> transmit);
        const std::vector<std::shared_ptr<SmallTransmit>>& getTransmitTo();
};

#endif