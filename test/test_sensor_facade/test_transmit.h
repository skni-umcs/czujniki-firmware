#ifndef TESTTRANSMIT_T
#define TESTTRANSMIT_T

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