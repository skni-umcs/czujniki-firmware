#ifndef DEBUGTIMETRANSMIT_H
#define DEBUGTIMETRANSMIT_H

#include <string>
#include <exchange/transmits/transmit.h>
#include "small_transmit.h"
#include <LoRa_E220.h>
#include "time/timer.h"
#include "message/message.h"
#include "wifi_transmit.h"

class DEBUG_timeTransmit : public SmallTransmit
{
  std::shared_ptr<Timer> timer = Timer::create();
  int noisedB;

  public:
    static std::shared_ptr<DEBUG_timeTransmit> create(std::shared_ptr<WifiTransmit> DEBUG_wifi);
    void setup();
    OperationResult poll();
    OperationResult send(std::string message, moduleAddress destinationNode) override;
    OperationResult send(std::shared_ptr<Message> message) override;
    OperationResult receive(std::shared_ptr<Message> message) override;
    OperationResult updateNoise();
    int getSnr(int readRssi);
    TransmitType type() const override {
      return TransmitType::OtherTransmit;
    }
};

#endif