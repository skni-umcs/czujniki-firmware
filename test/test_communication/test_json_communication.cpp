#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/jsonCommunication.h>
#include <exchange/transmits/wifiTransmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

std::shared_ptr<JsonCommunication> jsonCommunication;
std::unique_ptr<JsonTransmit> jsonTransmit;

void setup() {
    jsonCommunication = JsonCommunication::create();
    jsonTransmit = std::unique_ptr<JsonTransmit>(new WifiTransmit());
UNITY_BEGIN();
    jsonCommunication->subscribe(std::move(jsonTransmit));
    TEST_ASSERT_TRUE(jsonCommunication->getTransmitTo().size() == 1);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
