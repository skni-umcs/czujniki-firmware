#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"

class LoraTransmit : public JsonTransmit
{
    public:
        OperationResult send(std::string message) override;
        OperationResult receive(std::string message) override;
};

#endif