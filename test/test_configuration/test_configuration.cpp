#include <unity.h>
#include "utils/configuration_facade.h"
#include "mock_transmit.h"

void test_null_sensor_facade() {
    ConfigurationFacade configuration;
    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setServicePeriodMs(100, true));
    TEST_ASSERT_EQUAL(CONFIGURATION_ERROR_INT, configuration.getServicePeriodMs());
}

void test_set_service_period_no_save_and_reset() {
    ConfigurationFacade configuration1 = ConfigurationFacade();
    auto sensor1 = SensorFacade::create(std::shared_ptr<SmallTransmit>(new MockTransmit()));
    configuration1.plugSensorFacade(sensor1);
    TEST_ASSERT_EQUAL(DEFAULT_SERVICE_PERIOD_MS, configuration1.getServicePeriodMs());
    TEST_ASSERT_EQUAL(OperationResult::SUCCESS, configuration1.setServicePeriodMs(12345, false));
    TEST_ASSERT_EQUAL(12345, configuration1.getServicePeriodMs());
}

void test_null_service_communication() {
    ConfigurationFacade configuration;
    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setAskTimeoutMs(100, true));
    TEST_ASSERT_EQUAL(CONFIGURATION_ERROR_INT, configuration.getAskTimeoutMs());

    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setTimeSyncPeriodMs(5000, true));
    TEST_ASSERT_EQUAL(CONFIGURATION_ERROR_INT, configuration.getTimeSyncPeriodMs());
}

void test_set_ask_timeout_no_save_and_get() {
    ConfigurationFacade configuration;
    auto service = ServiceCommunication::create();
     std::cout << "dwadwadwdadwwad plug" << std::endl;
    configuration.plugServiceCommunication(service);
    std::cout << "after plug" << std::endl;

    TEST_ASSERT_EQUAL(DEFAULT_ASK_TIMEOUT_MS, configuration.getAskTimeoutMs());
    std::cout << "rrrrrrrrrrrrrrrrrrrrrrrrr" << std::endl;
    TEST_ASSERT_EQUAL(OperationResult::SUCCESS, configuration.setAskTimeoutMs(9876, false));
    std::cout << "rrrrrrrrrrrrrrrrrrrrrrrrr" << std::endl;
    TEST_ASSERT_EQUAL(9876, configuration.getAskTimeoutMs());
}

void test_set_time_sync_and_save() {
    ConfigurationFacade configuration;
    auto service = ServiceCommunication::create();
    configuration.plugServiceCommunication(service);

    TEST_ASSERT_EQUAL(DEFAULT_TIME_SYNC_PERIOD_MS, configuration.getTimeSyncPeriodMs());
    TEST_ASSERT_EQUAL(OperationResult::SUCCESS, configuration.setTimeSyncPeriodMs(123456, true));
    TEST_ASSERT_EQUAL(123456, configuration.getTimeSyncPeriodMs());
}

void test_set_invalid_values() {
    ConfigurationFacade configuration;
    auto service = ServiceCommunication::create();
    configuration.plugServiceCommunication(service);

    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setAskTimeoutMs(CONFIGURATION_ERROR_INT, true));
    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setTimeSyncPeriodMs(CONFIGURATION_ERROR_INT, true));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_null_sensor_facade);
    RUN_TEST(test_set_service_period_no_save_and_reset);
    RUN_TEST(test_null_service_communication);
    RUN_TEST(test_set_ask_timeout_no_save_and_get);
    RUN_TEST(test_set_time_sync_and_save);
    RUN_TEST(test_set_invalid_values);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
