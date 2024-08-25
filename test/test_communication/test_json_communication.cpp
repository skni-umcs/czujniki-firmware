#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/jsonCommunication.h>
#include <exchange/transmits/wifiTransmit.h>
#include <iostream>
#include <memory>

std::shared_ptr<JsonCommunication> jsonCommunication;
std::unique_ptr<JsonTransmit> jsonTransmit;

void setup() {
    jsonCommunication = JsonCommunication::create();
    jsonTransmit = std::unique_ptr<JsonTransmit>(new WifiTransmit());
}

void loop() {
    // clean stuff up here
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
    jsonCommunication->subscribe(std::move(jsonTransmit));
    TEST_ASSERT_TRUE(jsonCommunication->getTransmitTo().size() == 1);
    UNITY_END();
}