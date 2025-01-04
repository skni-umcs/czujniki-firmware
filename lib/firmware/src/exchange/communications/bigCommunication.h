#ifndef BIGCOMMUNICATION_H
#define BIGCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/bigTransmit.h>
#include "types/loraTypes.h"

class BigCommunication : public Communication {
    std::vector<std::shared_ptr<BigTransmit>> transmitTo;
    public:
        std::shared_ptr<Communication> create();
        OperationResult getNotified(Message message) override;
        OperationResult transmit(Message message) override;
        virtual OperationResult subscribe(std::shared_ptr<BigTransmit> bigTransmit) = 0;
        virtual OperationResult unsubscribe(std::shared_ptr<BigTransmit> bigTransmit) = 0;
};

#endif