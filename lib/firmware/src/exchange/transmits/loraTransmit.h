#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"
#include <LoRa_E220.h>

const int TX_PIN = 17;
const int RX_PIN = 16;
const int AUX_PIN = 18;
const int M0_PIN = 21;
const int M1_PIN = 19;

class LoraTransmit : public JsonTransmit
{
    LoRa_E220 e220ttl = LoRa_E220(TX_PIN, RX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);
    public:
        static std::shared_ptr<LoraTransmit> create();
        void setup();
        OperationResult send(std::string message) override;
        OperationResult receive(std::string message) override;
};

#endif