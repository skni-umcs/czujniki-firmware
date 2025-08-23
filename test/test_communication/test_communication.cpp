#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/transmits/wifi_transmit.h>
#include <exchange/transmits/lora_transmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <exchange/communications/passthrough_communication.h>
#include <exchange/communications/service_communication.h>
#include "time/time_constants.h"
#include "mock_transmit.h"
#include <exchange/communications/sensor_communication.h>
#include <exchange/communications/update_communication.h>
#include <exchange/communications/config_communication.h>
#include "utils/address_handler.h"

const moduleAddress TEST_MODULE_ADDRESS = 28;

class MockAddressHandler : public AddressHandler {
    public:
        moduleAddress readAddress() override;
};

moduleAddress MockAddressHandler::readAddress() {
    return TEST_MODULE_ADDRESS;
}

void setupAddressHandler() {
    AddressHandler::_DEBUG_setInstance(std::shared_ptr<AddressHandler>(new MockAddressHandler()));
}

void test_subscription_small(std::shared_ptr<SmallCommunication> communication, std::shared_ptr<SmallTransmit> transmit) {
    communication->subscribe(transmit);
    TEST_ASSERT_EQUAL(1, communication->getTransmitTo().size());
}

void test_subscription_big(std::shared_ptr<BigCommunication> communication, std::shared_ptr<BigTransmit> transmit) {
    communication->subscribe(transmit);
    TEST_ASSERT_EQUAL(1, communication->getTransmitTo().size());
}

void test_sensor_subscription() {
    test_subscription_small(
        SensorCommunication::create(),
        MockTransmit::create()
    );
}

void test_passthrough_subscription() {
    test_subscription_small(
        PassthroughCommunication::create(),
        MockTransmit::create()
    );
}

void test_service_subscription() {
    test_subscription_small(
        ServiceCommunication::create(),
        MockTransmit::create()
    );
}

void test_update_subscription() {
    test_subscription_big(
        UpdateCommunication::create(),
        MockTransmit::create()
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
    std::vector<std::shared_ptr<LoraMessage>> messages;
    passthroughCommunication->removeSameMessages(messages, message);
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

void test_passthrough_update_set_from_new_message() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -10;
    int currentRssi = 250;
    auto oldMessage = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr));
    auto newMessage = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr));
    passthroughCommunication->getNotified(oldMessage);
    TEST_ASSERT_TRUE(oldMessage->getShouldTransmit());
    TEST_ASSERT_EQUAL(1, passthroughCommunication->getMessageSetLength());
    TEST_ASSERT_TRUE(passthroughCommunication->getIsSendWaiting());
    passthroughCommunication->updateSetFromNewMessage(newMessage);
    TEST_ASSERT_FALSE(oldMessage->getShouldTransmit());
    TEST_ASSERT_EQUAL(0, passthroughCommunication->getMessageSetLength());
}

void test_passthrough_after_wait() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    auto message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "1", rssi, hopLimit, currentRssi, snr));
    auto message2 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "2", rssi, hopLimit, currentRssi, snr));
    auto message3 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "3", rssi, hopLimit, currentRssi, snr));
    passthroughCommunication->getNotified(message1);
    passthroughCommunication->getNotified(message2);
    passthroughCommunication->getNotified(message3);
}

void test_passthrough_ponder_empty_loop() {
    passthroughCommunication->ponderAfterWait(true);

    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    auto message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, 1, "1", rssi, hopLimit, currentRssi, snr));
    passthroughCommunication->ponderAfterWait(true);
}


void test_service_config_change() {
    auto serviceCommunication = ServiceCommunication::create();
    auto sensorFacade = SensorFacade::create(MockTransmit::create(), SensorCommunication::create(), serviceCommunication);
    auto configCommunication = ConfigCommunication::create(sensorFacade, serviceCommunication);

    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    auto message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, TEST_MODULE_ADDRESS, "t501", rssi, hopLimit, currentRssi, snr));
    configCommunication->getNotified(message1);
    TEST_ASSERT_EQUAL(501*1000, sensorFacade->getTelemetryPeriodMs());

    message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, TEST_MODULE_ADDRESS, "s502", rssi, hopLimit, currentRssi, snr));
    configCommunication->getNotified(message1);
    TEST_ASSERT_EQUAL(502*1000, sensorFacade->getServicePeriodMs());

    message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, TEST_MODULE_ADDRESS, "a31", rssi, hopLimit, currentRssi, snr));
    configCommunication->getNotified(message1);
    TEST_ASSERT_EQUAL(31*1000, serviceCommunication->getAskTimeoutMs());

    message1 = std::shared_ptr<LoraMessage>(new LoraMessage(senders, TEST_MODULE_ADDRESS, "p32", rssi, hopLimit, currentRssi, snr));
    configCommunication->getNotified(message1);
    TEST_ASSERT_EQUAL(32*1000, serviceCommunication->getTimeSyncPeriodMs());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_subscription);
    RUN_TEST(test_passthrough_subscription);
    RUN_TEST(test_service_subscription);
    RUN_TEST(test_update_subscription);
    RUN_TEST(test_passthrough_wrong_message_type);
    RUN_TEST(test_passthrough_no_message_in_set);
    RUN_TEST(test_service_time_update);
    RUN_TEST(test_service_time_update_ignore_didnt_ask);
    RUN_TEST(test_passthrough_update_set_from_new_message);
    RUN_TEST(test_passthrough_after_wait);
    RUN_TEST(test_passthrough_ponder_empty_loop);
    RUN_TEST(test_service_config_change);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
