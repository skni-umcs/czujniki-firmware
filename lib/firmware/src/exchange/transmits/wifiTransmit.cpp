#include <utils/operationResult.h>
#include "wifiTransmit.h"
#include <iostream>
#include <WiFi.h>
#include <memory>
#include "utils/SPIFFSUtils.h"

#define MINIMUM_RSSI -300
#define NO_NETWORK -1
#define NO_NETWORK_SSID ""

const int POLL_PERIOD_MS = 100; // low value for testing
WiFiServer server(80);

//TODO: rewrite connection logic, reverse 
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
    Serial.println("[+] Connected to the WiFi network");
}

void disconnected_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Serial.println("[-] Disconnected from the WiFi AP");
    WiFi.reconnect();
}

void got_ip_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Serial.print("[+] Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
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
        Serial.println("New Client Connected.");
        unsigned long startTime = millis();
        while (millis() - startTime < 100 && !client.available()) { }
        String idMsg = "0";
        moduleAddress nodeId = parseNodeId(idMsg);
        activeClients[nodeId] = client;
        Serial.printf("Client with node id %d registered.\n", nodeId);
    }

    for (auto it = activeClients.begin(); it != activeClients.end(); ) {
        if (!it->second.connected()) {
            Serial.printf("Client with node id %d disconnected.\n", it->first);
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
            Serial.print("Received: ");
            Serial.println(message);
        }
    }

    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::send(std::string message, moduleAddress destinationNode) {
    auto it = activeClients.find(destinationNode);
    if (it != activeClients.end() && it->second.connected()) {
        it->second.print(message.c_str());
        return OperationResult::SUCCESS;
    }
    return OperationResult::ERROR;
}

OperationResult WifiTransmit::send(std::shared_ptr<Message> message) {
    moduleAddress dest = message->getDestination();
    auto it = activeClients.find(dest);
    if (it != activeClients.end() && it->second.connected()) {
        it->second.print(message->getContent().c_str());
        return OperationResult::SUCCESS;
    }
    return OperationResult::ERROR;
}

OperationResult WifiTransmit::receive(std::shared_ptr<Message> message) {
    Serial.printf("WIFI RECEIVE %s\n", message->getPacket().c_str());
    notifySubscribers(message);
    return OperationResult::SUCCESS;
}

std::map<String, String> WifiTransmit::getNetworks() {
    return networks;
}
