#ifndef LORATRANSMIT_H
#define LORATRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "jsonTransmit.h"
#include <LoRa_E220.h>

class LoraTransmit : public JsonTransmit
{
    LoRa_E220 e220ttl = LoRa_E220(17, 16, &Serial2, 18, 21, 19, UART_BPS_RATE_9600);
    public:
        static std::shared_ptr<LoraTransmit> create();
        void setup();
        OperationResult send(std::string message) override;
        OperationResult receive(std::string message) override;
};

#endif