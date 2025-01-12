#ifndef JSONCOMMUNICATION_H
#define JSONCOMMUNICATION_H

#include <vector>
#include <exchange/transmits/jsonTransmit.h>
#include "communication.h"
#include <memory>
#include "utils/storageTypes.h"

class JsonCommunication : public Communication {
    
    protected:
        std::vector<std::shared_ptr<JsonTransmit>> transmitTo;
    public:
        static std::shared_ptr<JsonCommunication> create();
        virtual OperationResult getNotified(std::shared_ptr<Message> message) override;
        OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        OperationResult subscribe(std::shared_ptr<JsonTransmit> transmit);
        OperationResult unsubscribe(std::shared_ptr<JsonTransmit> transmit);
        const std::vector<std::shared_ptr<JsonTransmit>>& getTransmitTo();
};

#endif