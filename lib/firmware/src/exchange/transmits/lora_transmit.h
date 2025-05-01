#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "small_transmit.h"
#include <LoRa_E220.h>
#include "time/timer.h"
#include "message/message.h"
#include <deque>
#include <time/waiter.h>
#include "wifi_transmit.h"

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

class LoraTransmit : public SmallTransmit
{
  LoRa_E220 e220ttl = LoRa_E220(ESP_RX_PIN, ESP_TX_PIN, &Serial1, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);
  std::shared_ptr<Timer> pollTimer = Timer::create(POLL_TIMER_PRIORITY);
  std::shared_ptr<Timer> noiseUpdateTimer = Timer::create();
  int noiseRaw; //TODO: think about initial value of noise in case of error?
  std::deque<std::shared_ptr<Message>> messages;
  bool canTransmit = true;
  std::shared_ptr<Waiter> sendWaiter = Waiter::create();
  std::shared_ptr<WifiTransmit> DEBUG_wifi;

  public:
    static std::shared_ptr<LoraTransmit> create(std::shared_ptr<WifiTransmit> DEBUG_wifi);
    void setup();
    OperationResult poll();
    OperationResult physicalSend(std::shared_ptr<Message> message);
    OperationResult advanceMessages();
    OperationResult send(std::string message, moduleAddress destinationNode) override;
    OperationResult send(std::shared_ptr<Message> message) override;
    OperationResult scheduleMessage(std::shared_ptr<Message> message);
    OperationResult receive(std::shared_ptr<Message> message) override;
    OperationResult updateNoise();
    int getSnr(int readRssi);
    int getNoise();
    bool getCanTransmit();
    int getWaitingMessagesCount();
    OperationResult RENAMEadvanceMessages();
    TransmitType type() const override {
      return TransmitType::LoraTransmit;
    }
};

#endif