#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/small_communication.h>
#include <exchange/transmits/wifi_transmit.h>
#include <exchange/transmits/lora_transmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <time/timer.h>

void test_is_same_message() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    auto oldMessage = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr));
    auto newMessage = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr));
    TEST_ASSERT_TRUE(oldMessage->isSameMessage(newMessage));
    TEST_ASSERT_TRUE(newMessage->isSameMessage(oldMessage));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_is_same_message);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
