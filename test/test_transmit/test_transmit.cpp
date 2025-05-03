#include <unity.h>
#include <string>
#include <algorithm> 
#include "mock_small_communication.h"
#include "mock_big_communication.h"
#include <exchange/transmits/wifi_transmit.h>
#include <exchange/transmits/DEBUG_time_transmit.h>
#include <exchange/transmits/lora_transmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <exchange/communications/passthrough_communication.h>
#include <exchange/communications/service_communication.h>
#include "time/time_constants.h"

void test_subscription_small(std::shared_ptr<SmallCommunication> communication, std::shared_ptr<SmallTransmit> transmit) {
    communication->subscribe(transmit);
    TEST_ASSERT_EQUAL(1, communication->getTransmitTo().size());
}

void test_subscription_big(std::shared_ptr<BigCommunication> communication, std::shared_ptr<BigTransmit> transmit) {
    communication->subscribe(transmit);
    TEST_ASSERT_EQUAL(1, communication->getTransmitTo().size());
}

void test_wifi_subscription_small() {
    test_subscription_small(
        MockSmallCommunication::create(),
        WifiTransmit::create()
    );
}

void test_wifi_subscription_big() {
    test_subscription_big(
        MockBigCommunication::create(),
        WifiTransmit::create()
    );
}

void test_DEBUG_subscription_small() {
    test_subscription_small(
        MockSmallCommunication::create(),
        DEBUG_timeTransmit::create(WifiTransmit::create())
    );
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_subscription_small);
    RUN_TEST(test_wifi_subscription_big);
    RUN_TEST(test_DEBUG_subscription_small);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
