#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/jsonCommunication.h>
#include <exchange/transmits/wifiTransmit.h>
#include <exchange/transmits/loraTransmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

void test_subscription(std::shared_ptr<JsonCommunication> jsonCommunication, std::unique_ptr<JsonTransmit> jsonTransmit) {
    jsonCommunication->subscribe(std::move(jsonTransmit));
    TEST_ASSERT_EQUAL(jsonCommunication->getTransmitTo().size(), 1);
}

void test_wifi_subscription() {
    test_subscription(
        JsonCommunication::create(),
        std::unique_ptr<JsonTransmit>(new WifiTransmit())
    );
}

void test_lora_subscription() {
    test_subscription(
        JsonCommunication::create(),
        std::unique_ptr<JsonTransmit>(new LoraTransmit())
    );
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_subscription);
    RUN_TEST(test_lora_subscription);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
