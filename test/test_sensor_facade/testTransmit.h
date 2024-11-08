#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "exchange/transmits/jsonTransmit.h"

class TestTransmit : public JsonTransmit
{
    public:
        OperationResult send(std::string message) override;
        OperationResult receive(std::string message) override;
};

#endif