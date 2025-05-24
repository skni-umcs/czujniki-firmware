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

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_null_sensor_facade);
    RUN_TEST(test_set_service_period_no_save_and_reset);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
