#ifndef SENSORCOMMUNICATION_H
#define SENSORCOMMUNICATION_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/small_transmit.h>

class SensorCommunication : public SmallCommunication {
    
    protected:
        SensorCommunication() {};

    public:
        static std::shared_ptr<SensorCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
};

#endif