#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"
#include <LoRa_E220.h>
#include "time/timer.h"
#include "utils/storageTypes.h"

#if defined(esp32firebeetle)
#define ESP_RX_PIN 17
#define ESP_TX_PIN 16
#define AUX_PIN 18
#define M0_PIN 21
#define M1_PIN 19
#elif defined(mini_test)
#define ESP_RX_PIN 16
#define ESP_TX_PIN 17
#define AUX_PIN 18
#define M0_PIN 21
#define M1_PIN 12
#else
#define ESP_RX_PIN 7
#define ESP_TX_PIN 6
#define AUX_PIN 5
#define M0_PIN 1
#define M1_PIN 2
#endif

const int POLL_TIMER_PRIORITY = 1;

class LoraTransmit : public JsonTransmit
{
  LoRa_E220 e220ttl = LoRa_E220(ESP_RX_PIN, ESP_TX_PIN, &Serial1, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);
  std::shared_ptr<Timer> timer = Timer::create(POLL_TIMER_PRIORITY);
  int noiseRaw;

  public:
    static std::shared_ptr<LoraTransmit> create();
    void setup();
    OperationResult poll();
    OperationResult send(std::string message, moduleAddress destinationNode) override;
    OperationResult send(std::shared_ptr<Message> message) override;
    OperationResult receive(std::shared_ptr<Message> message) override;
    OperationResult updateNoise();
    int getSnr(int readRssi);
    int getNoise();
};

#endif