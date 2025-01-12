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
#include <exchange/communications/passthroughCommunication.h>

void test_subscription(std::shared_ptr<JsonCommunication> jsonCommunication, std::unique_ptr<JsonTransmit> jsonTransmit) {
    jsonCommunication->subscribe(std::move(jsonTransmit));
    TEST_ASSERT_EQUAL(1, jsonCommunication->getTransmitTo().size());
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

auto passthroughCommunication = PassthroughCommunication::create();

void test_wrong_message_type() {
    auto senders = std::vector<moduleAddress>{55};
    auto rssi = std::vector<std::string>{};
    auto hopLimit = 100;
    auto message = std::shared_ptr<GeneratedMessage>(new GeneratedMessage(senders, 1, "test", rssi, hopLimit));
    TEST_ASSERT_EQUAL(OperationResult::ERROR, passthroughCommunication->getNotified(message));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_subscription);
    RUN_TEST(test_lora_subscription);
    RUN_TEST(test_wrong_message_type);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
