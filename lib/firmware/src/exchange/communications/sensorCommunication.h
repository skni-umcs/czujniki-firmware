#ifndef SENSORCOMMUNICATION_H
#define SENSORCOMMUNICATION_H

#include <vector>
#include "jsonCommunication.h"
#include <exchange/transmits/jsonTransmit.h>

class SensorCommunication : public JsonCommunication {
    
    public:
        static std::shared_ptr<SensorCommunication> create();
};

#endif