#ifndef SERVICECOMMUNICATION_H
#define SERVICECOMMUNICATION_H

#include <vector>
#include "jsonCommunication.h"
#include <exchange/transmits/jsonTransmit.h>

class ServiceCommunication : public JsonCommunication {
    
    public:
        static std::shared_ptr<ServiceCommunication> create();
};

#endif