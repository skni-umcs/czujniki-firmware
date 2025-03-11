#include <utils/operationResult.h>
#include "wifiTransmit.h"
#include <iostream>
#include <WiFi.h>
#include <memory>
#include "utils/SPIFFSUtils.h"

#define MINIMUM_RSSI -300
#define NO_NETWORK -1
#define NO_NETWORK_SSID ""

const int POLL_PERIOD_MS = 100; //low value for testing
WiFiServer server(80); // Port 80 for HTTP, can be other

bool WifiTransmit::isKnownNetwork(String ssid) {
    return networks.find(ssid) != networks.end();
}

void WifiTransmit::setupPollTask() {
    pollTimer.get()->setExecuteFunction([this]() {
        this->poll();
    });
    pollTimer->updateTime(POLL_PERIOD_MS);
}

std::shared_ptr<WifiTransmit> WifiTransmit::create() {
    auto wifiTransmit = std::shared_ptr<WifiTransmit>(new WifiTransmit());
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

void wifiInitTask(void* wifiTransmitPointer) {
    vTaskDelay(15000); //temporary sleep thats enough, lower sleeps block other tasks
    WiFi.mode(WIFI_STA);

    WiFi.onEvent(connected_to_ap, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(got_ip_from_ap, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(disconnected_from_ap, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    std::shared_ptr<WifiTransmit>* wifiTransmit = static_cast<std::shared_ptr<WifiTransmit>*>(wifiTransmitPointer);
    String bestNetworkSsid = wifiTransmit->get()->getBestNetworkSsid();
    if(bestNetworkSsid != NO_NETWORK_SSID) {
        WiFi.begin(bestNetworkSsid, wifiTransmit->get()->getNetworks().at(bestNetworkSsid));
        server.begin();
    }

    wifiTransmit->get()->setupPollTask();

    vTaskDelete(NULL);
}

OperationResult WifiTransmit::setup() {
    networks = retrieveNetworks();
    auto* taskPtr = new std::shared_ptr<WifiTransmit>(shared_from_this());

    const int bytesNeeded = 25600; //temporary value thats working
    const char* taskName = "wifiInitTask";
    void* taskArgument = static_cast<void*>(taskPtr);
    TaskHandle_t* const taskHandle = nullptr;
    xTaskCreate(wifiInitTask, taskName, bytesNeeded, taskArgument, 1, taskHandle);
    return OperationResult::SUCCESS;
}

OperationResult WifiTransmit::poll() {
    //TODO: rewrite to events
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client Connected.");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                receive(std::shared_ptr<TextMessage>(new TextMessage(std::string(message.c_str()))));
                Serial.print("Received: ");
                Serial.println(message);
            }
        }
        client.stop();
        Serial.println("Client Disconnected.");
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
	Serial.printf("RECEIVE WIFI %s\n", message->getPacket().c_str());
	notifySubscribers(message);
    return OperationResult::SUCCESS;
}

std::map<String, String> WifiTransmit::getNetworks() {
	return networks;
}