#ifndef BIGCOMMUNICATION_H
#define BIGCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/big_transmit.h>
#include "utils/storage_types.h"

class BigCommunication : public Communication {
    std::vector<std::shared_ptr<BigTransmit>> transmitTo;
    public:
        std::shared_ptr<Communication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) = 0;
        virtual OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        virtual OperationResult transmit(std::shared_ptr<Message> message) override;
        OperationResult subscribe(std::shared_ptr<BigTransmit> bigTransmit);
        OperationResult unsubscribe(std::shared_ptr<BigTransmit> bigTransmit);
        const std::vector<std::shared_ptr<BigTransmit>>& getTransmitTo();
};

#endif