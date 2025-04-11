#ifndef SENSORCOMMUNICATION_H
#define SENSORCOMMUNICATION_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/small_transmit.h>

class SensorCommunication : public SmallCommunication {
    
    public:
        static std::shared_ptr<SensorCommunication> create();
        OperationResult transmit(std::string message, moduleAddress destinationNode) override;
        OperationResult transmit(std::shared_ptr<Message> message) override;
        OperationResult getNotified(std::shared_ptr<Message> message) override;
};

#endif