#ifndef PASSTHROUGHCOMMUNICATION_H
#define PASSTHROUGHCOMMUNICATION_H

#include "jsonCommunication.h"

class PassthroughCommunication : public JsonCommunication {
    
    public:
        static std::shared_ptr<PassthroughCommunication> create();
};

#endif