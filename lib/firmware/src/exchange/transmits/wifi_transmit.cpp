#include <utils/operation_result.h>
#include "wifi_transmit.h"
#include <iostream>
#include <WiFi.h>
#include <memory>
#include "utils/spiffs_utils.h"
#include "exchange/communications/small_communication.h"
#include "exchange/transmits/big_transmit.h"
#include "exchange/communications/big_communication.h"
#include <utils/logger.h>

#define MINIMUM_RSSI -300
#define NO_NETWORK -1
#define NO_NETWORK_SSID ""

const int POLL_PERIOD_MS = 100; // low value for testing
WiFiServer server(80);

moduleAddress parseNodeId(const String &idMsg) {
    return idMsg.toInt();
}

std::map<moduleAddress, WiFiClient> activeClients;

bool WifiTransmit::isKnownNetwork(String ssid) {
    return networks.find(ssid) != networks.end();
}

std::shared_ptr<WifiTransmit> WifiTransmit::create() {
    auto wifiTransmit = std::shared_ptr<WifiTransmit>(new WifiTransmit());
    wifiTransmit->setup();
    return wifiTransmit;
}

String WifiTransmit::getBestNetworkSsid() {
    int networksCount = WiFi.scanNetworks();
    int32_t maximumRssi = MINIMUM_RSSI;
    int bestNetworkIndex = NO_NETWORK;
    for (int i = 0; i < networksCount; ++i) {
        String ssid = WiFi.SSID(i);
        if (isKnownNetwork(ssid)) {
            int32_t rssi = WiFi.RSSI();
            if (rssi > maximumRssi) {
                maximumRssi = rssi;
                bestNetworkIndex = i;
            }
        }
    }
    if (bestNetworkIndex == NO_NETWORK) {
        return NO_NETWORK_SSID;
    }
    return WiFi.SSID(bestNetworkIndex);
}

void connected_to_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Logger::log("[+] Connected to the WiFi network");
}

void disconnected_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Logger::log("[-] Disconnected from the WiFi AP");
    WiFi.reconnect();
}

void got_ip_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Logger::log("[+] Local ESP32 IP: ");
    Logger::log(WiFi.localIP());
}

void WifiTransmit::setupPollTask() {
    pollTimer->setExecuteFunction([this]() {
        this->poll();
    });
    pollTimer->updateTime(POLL_PERIOD_MS);
}

OperationResult WifiTransmit::setup() {
    networks = retrieveNetworks();
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(connected_to_ap, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(got_ip_from_ap, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(disconnected_from_ap, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    String bestNetworkSsid = getBestNetworkSsid();
    if (bestNetworkSsid != NO_NETWORK_SSID) {
        WiFi.begin(bestNetworkSsid, getNetworks().at(bestNetworkSsid));
        server.begin();
    }

    setupPollTask();
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::poll() {
    WiFiClient client = server.available();
    if (client) {
        Logger::log("New Client Connected.");
        unsigned long startTime = millis();
        while (millis() - startTime < 100 && !client.available()) { }
        String idMsg = "0";
        moduleAddress nodeId = parseNodeId(idMsg);
        activeClients[nodeId] = client;
        Logger::logf("Client with node id %d registered.\n", nodeId);
    }

    for (auto it = activeClients.begin(); it != activeClients.end(); ) {
        if (!it->second.connected()) {
            Logger::logf("Client with node id %d disconnected.\n", it->first);
            it = activeClients.erase(it);
        } else {
            ++it;
        }
    }

    for (auto &pair : activeClients) {
        WiFiClient &clientRef = pair.second;
        while (clientRef.available()) {
            String message = clientRef.readStringUntil('\n');
            receive(std::make_shared<TextMessage>(std::string(message.c_str())));
            Logger::log("Received: ");
            Logger::log(message);
        }
    }

    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::send(std::shared_ptr<Message> message) {
    moduleAddress dest = message->getDestination();
    std::string messageContent = message->createPacketForSending();

    auto it = activeClients.find(dest);
    if (it != activeClients.end() && it->second.connected()) {
        it->second.printf("%s\n", messageContent.c_str());
        return OperationResult::SUCCESS;
    }
    return OperationResult::ERROR;
}

OperationResult WifiTransmit::receive(std::shared_ptr<Message> message) {
    Logger::logf("WIFI RECEIVE %s\n", message->getPacket().c_str());
    notifySubscribers(message);
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::notifySubscribers(std::shared_ptr<Message> message) {
    OperationResult smallResult = this->SmallTransmit::notifySubscribers(message);
    OperationResult bigResult = this->BigTransmit::notifySubscribers(message);
    if(smallResult == OperationResult::SUCCESS && bigResult == OperationResult::SUCCESS) {
        return OperationResult::SUCCESS;
    }
    return OperationResult::ERROR;
}

std::map<String, String> WifiTransmit::getNetworks() {
    return networks;
}
