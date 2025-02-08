#ifndef DEBUGTIMETRANSMIT_H
#define DEBUGTIMETRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"
#include <LoRa_E220.h>
#include "time/timer.h"
#include "utils/storageTypes.h"

class DEBUG_timeTransmit : public JsonTransmit
{
  std::shared_ptr<Timer> timer = Timer::create();
  int noisedB;

  public:
    static std::shared_ptr<DEBUG_timeTransmit> create();
    void setup();
    OperationResult poll();
    OperationResult send(std::string message, moduleAddress destinationNode) override;
    OperationResult send(std::shared_ptr<Message> message) override;
    OperationResult receive(std::shared_ptr<Message> message) override;
    OperationResult updateNoise();
    int getSnr(int readRssi);
};

#endif