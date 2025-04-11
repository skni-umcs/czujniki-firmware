#ifndef DEBUGTIMETRANSMIT_H
#define DEBUGTIMETRANSMIT_H

#include <string>
#include <exchange/transmits/small_transmit.h>
#include <exchange/transmits/big_transmit.h>
#include <LoRa_E220.h>
#include "time/timer.h"
#include "utils/message.h"

class MockTransmit : public SmallTransmit, public BigTransmit
{
  public:
    static std::shared_ptr<MockTransmit> create();
    virtual OperationResult send(std::string message, moduleAddress destinationNode) override;
    virtual OperationResult send(std::shared_ptr<Message> message) override;
    virtual OperationResult receive(std::shared_ptr<Message> message) override;
};

#endif