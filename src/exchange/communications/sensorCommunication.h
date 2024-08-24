#ifndef SENSORCOMMUNICATION_H
#define SENSORCOMMUNICATION_H

#include "sensor.h"
#include <vector>
#include "jsonCommunication.h"
#include <exchange/transmits/jsonTransmit.h>

class SensorCommunication : JsonCommunication {
    public:
        std::string getNotified(std::string message) override;
        void subscribe(std::unique_ptr<JsonTransmit> transmit) override;
        void unsubscribe(std::unique_ptr<JsonTransmit> transmit) override;
};

#endif