#ifndef BIGCOMMUNICATION_H
#define BIGCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/bigTransmit.h>

class BigCommunication : public Communication {
    std::vector<std::shared_ptr<BigTransmit>> transmitTo;
    public:
        std::shared_ptr<Communication> create();
        OperationResult getNotified(std::string message) override;
        OperationResult transmit(std::string message) override;
        virtual OperationResult subscribe(std::shared_ptr<BigTransmit> bigTransmit) = 0;
        virtual OperationResult unsubscribe(std::shared_ptr<BigTransmit> bigTransmit) = 0;
};

#endif