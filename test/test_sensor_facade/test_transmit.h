#ifndef TESTTRANSMIT_T
#define TESTTRANSMIT_T

#include <string>
#include <exchange/communications/communication.h>
#include "exchange/transmits/small_transmit.h"
#include "message/message.h"

class TestTransmit : public SmallTransmit
{
    public:
        OperationResult send(std::string message, moduleAddress moduleAddress) override;
        OperationResult send(std::shared_ptr<Message> message) override;
        OperationResult receive(std::shared_ptr<Message> message) override;
        TransmitType type() const override {
            return TransmitType::OtherTransmit;
        }
};

#endif