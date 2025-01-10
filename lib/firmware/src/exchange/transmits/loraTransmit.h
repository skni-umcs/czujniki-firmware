#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"
#include <LoRa_E220.h>
#include "time/timer.h"
#include "utils/storageTypes.h"

#ifdef esp32firebeetle
#define TX_PIN 17
#define RX_PIN 16
#define AUX_PIN 18
#define M0_PIN 21
#define M1_PIN 19
#elif test
#define TX_PIN 16
#define RX_PIN 17
#define AUX_PIN 18
#define M0_PIN 21
#define M1_PIN 12
#else
#define TX_PIN 6
#define RX_PIN 7
#define AUX_PIN 5
#define M0_PIN 1
#define M1_PIN 2
#endif

class LoraTransmit : public JsonTransmit
{
  LoRa_E220 e220ttl = LoRa_E220(TX_PIN, RX_PIN, &Serial1, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);
  std::shared_ptr<Timer> timer = Timer::create();

  public:
    static std::shared_ptr<LoraTransmit> create();
    void setup();
    OperationResult poll();
    OperationResult send(std::string message, moduleAddress destinationNode) override;
    OperationResult receive(std::shared_ptr<Message> message) override;
};

#endif