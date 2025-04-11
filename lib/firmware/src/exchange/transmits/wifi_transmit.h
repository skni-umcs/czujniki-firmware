#ifndef WIFITRANSMIT_H
#define WIFITRANSMIT_H

#include <string>
#include "big_transmit.h"
#include "small_transmit.h"
#include "utils/storage_types.h"
#include <time/timer.h>
#include <map>

class WifiTransmit : public BigTransmit, public SmallTransmit, public std::enable_shared_from_this<WifiTransmit>
{
    std::shared_ptr<Timer> pollTimer = Timer::create();
    std::map<String, String> networks;
    public:
        static std::shared_ptr<WifiTransmit> create();
        bool isKnownNetwork(String ssid);
        OperationResult send(std::string message, moduleAddress destinationNode) override;
        OperationResult send(std::shared_ptr<Message> message) override;
        OperationResult receive(std::shared_ptr<Message> message) override;
        String getBestNetworkSsid();
        OperationResult setup();
        OperationResult poll();
        std::map<String, String> getNetworks();
        void setupPollTask();
        OperationResult notifySubscribers(std::shared_ptr<Message> message);
};

#endif