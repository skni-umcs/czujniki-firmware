#ifndef LORAMESSAGE_H
#define LORAMESSAGE_H
#include "communicationTypes.h"

class LoraMessage : Message {
    moduleAddress sender;
    moduleAddress destination;
    public:
        bool isAddressInvalid() override;
        std::string getMessage() override;

}

#endif