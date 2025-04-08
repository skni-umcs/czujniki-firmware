#ifndef SENSORCOMMUNICATION_H
#define SENSORCOMMUNICATION_H

#include <vector>
#include "jsonCommunication.h"
#include <exchange/transmits/jsonTransmit.h>

class SensorCommunication : public JsonCommunication {
    
    public:
        static std::shared_ptr<SensorCommunication> create();
        OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        OperationResult transmit(std::shared_ptr<Message> message) override;
};

#endif