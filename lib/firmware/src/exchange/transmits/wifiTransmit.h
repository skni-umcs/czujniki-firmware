#ifndef WIFITRANSMIT_H
#define WIFITRANSMIT_H

#include <string>
#include <exchange/communications/communication.h>
#include "transmit.h"
#include "bigTransmit.h"
#include "utils/storageTypes.h"
#include <time/timer.h>

class WifiTransmit : public BigTransmit
{
    std::shared_ptr<Timer> pollTimer = Timer::create();
    public:
        static std::shared_ptr<WifiTransmit> create();
        OperationResult send(std::string message, moduleAddress destinationNode) override;
        OperationResult send(std::shared_ptr<Message> message) override;
        OperationResult receive(std::shared_ptr<Message> message) override;
        String getBestNetworkSsid();
        OperationResult setup();
        OperationResult poll();
};

#endif