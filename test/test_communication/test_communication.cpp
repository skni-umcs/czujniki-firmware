#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/json_communication.h>
#include <exchange/transmits/wifi_transmit.h>
#include <exchange/transmits/lora_transmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <exchange/communications/passthrough_communication.h>
#include <exchange/communications/service_communication.h>
#include "time/time_constants.h"

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

void test_passthrough_wrong_message_type() {
    auto senders = std::vector<moduleAddress>{55};
    auto rssi = std::vector<std::string>{};
    auto hopLimit = 100;
    auto message = std::shared_ptr<GeneratedMessage>(new GeneratedMessage(senders, 1, "test", rssi, hopLimit));
    TEST_ASSERT_EQUAL(OperationResult::ERROR, passthroughCommunication->getNotified(message));
}

void test_passthrough_no_message_in_set() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    auto message = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr));
    std::set<std::shared_ptr<LoraMessage>> set;
    passthroughCommunication->removeSameMessages(set, message);
}

void test_service_time_update() {
    auto serviceCommunication = ServiceCommunication::create();
    serviceCommunication->askForTime();
    unsigned long testEpoch = 1739024823;
    serviceCommunication->updateTime(testEpoch);
    TEST_ASSERT_TRUE(rtc.getEpoch() >= testEpoch);
}

void test_service_time_update_ignore_didnt_ask() {
    auto serviceCommunication = ServiceCommunication::create();
    unsigned long testEpoch = 2739024823;
    serviceCommunication->updateTime(testEpoch);
    TEST_ASSERT_TRUE(rtc.getEpoch() < testEpoch);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_subscription);
    RUN_TEST(test_lora_subscription);
    RUN_TEST(test_passthrough_wrong_message_type);
    RUN_TEST(test_passthrough_no_message_in_set);
    RUN_TEST(test_service_time_update);
    RUN_TEST(test_service_time_update_ignore_didnt_ask);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
