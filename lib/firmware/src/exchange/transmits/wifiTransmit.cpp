#include <utils/operationResult.h>
#include "wifiTransmit.h"
#include <iostream>
#include <WiFi.h>
#include "../src/Secrets.h"

#define MINIMUM_RSSI -300
#define NO_NETWORK -1
#define NO_NETWORK_SSID ""

bool isKnownNetwork(String ssid) {
    return networks.find(ssid) != networks.end();
}

std::shared_ptr<WifiTransmit> WifiTransmit::create() {
    auto wifiTransmit = new WifiTransmit();
    wifiTransmit->setup();
    return std::shared_ptr<WifiTransmit>{wifiTransmit};
}

String WifiTransmit::getBestNetworkSsid() {
    //TODO: rewrite to https://randomnerdtutorials.com/esp32-wifimulti/
    int networksCount = WiFi.scanNetworks();
    int32_t maximumRssi = MINIMUM_RSSI; 
    int bestNetworkIndex = NO_NETWORK;
    for(int i = 0;i<networksCount;++i) {
        String ssid = WiFi.SSID(i);
        if(isKnownNetwork(ssid)) {
            int32_t rssi = WiFi.RSSI();
            if(rssi > maximumRssi) {
                maximumRssi = rssi;
                bestNetworkIndex = i;
            }
        }
    }
    if(bestNetworkIndex == NO_NETWORK) {
        return NO_NETWORK_SSID;
    }
    return WiFi.SSID(bestNetworkIndex);
}


void connected_to_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Serial.println("[+] Connected to the WiFi network");
  }
  
  void disconnected_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Serial.println("[-] Disconnected from the WiFi AP");
    //TODO: add mechanism to periodically check if network is back online
    WiFi.reconnect();
    //WiFi.begin(ssid, password);
  }
  
  void got_ip_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    Serial.print("[+] Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

OperationResult WifiTransmit::setup() {
    WiFi.mode(WIFI_STA);

    WiFi.onEvent(connected_to_ap, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(got_ip_from_ap, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(disconnected_from_ap, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    String bestNetworkSsid = getBestNetworkSsid();
    if(bestNetworkSsid != NO_NETWORK_SSID) {
        WiFi.begin(bestNetworkSsid, networks.at(bestNetworkSsid));
    }
    else {
        return OperationResult::NOT_FOUND;
    }
    
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::send(std::string message, moduleAddress destinationNode) {
    std::cout << "wifi sending\n" << message << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::send(std::shared_ptr<Message> message) {
    std::cout << "wifi sending ready message\n" << message->getContent() << "\n" << std::flush;
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::receive(std::shared_ptr<Message> message) {
    return OperationResult::SUCCESS;
}
