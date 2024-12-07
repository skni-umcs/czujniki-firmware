#ifndef WIFITRANSMIT_H
#define WIFITRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"

class WifiTransmit : public JsonTransmit
{
    public:
        OperationResult send(std::string message, int destinationNode) override;
        OperationResult receive(std::string message) override;
};

#endif