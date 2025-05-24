#include <unity.h>
#include "utils/configuration_facade.h"
#include "mock_transmit.h"

void test_null_sensor_facade() {
    ConfigurationFacade configuration;
    // Without setting sensorFacade, calls should error
    TEST_ASSERT_EQUAL(OperationResult::ERROR, configuration.setServicePeriodMs(100, true));
    TEST_ASSERT_EQUAL(CONFIGURATION_ERROR_INT, configuration.getServicePeriodMs());
}

void test_set_service_period_no_save_and_reset() {
    ConfigurationFacade configuration1 = ConfigurationFacade();
    auto sensor1 = SensorFacade::create(std::shared_ptr<SmallTransmit>(new MockTransmit()));
    configuration1.plugSensorFacade(sensor1);

    // Set without saving
    TEST_ASSERT_EQUAL(OperationResult::SUCCESS, configuration1.setServicePeriodMs(12345, false));
    TEST_ASSERT_EQUAL(12345, configuration1.getServicePeriodMs());

    // Simulate controller reset: new facade & sensor, no load from prefs
    ConfigurationFacade configuration2;
    auto sensor2 = SensorFacade::create(std::shared_ptr<SmallTransmit>(new MockTransmit()));
    configuration2.plugSensorFacade(sensor2);
    // Should remain default after reset
    TEST_ASSERT_EQUAL(DEFAULT_SERVICE_PERIOD_MS, configuration2.getServicePeriodMs());
}

void test_set_service_period_with_save_and_reset() {
    ConfigurationFacade configuration1;
    auto sensor1 = SensorFacade::create(std::shared_ptr<SmallTransmit>(new MockTransmit()));
    configuration1.plugSensorFacade(sensor1);

    // Set with saving
    TEST_ASSERT_EQUAL(OperationResult::SUCCESS, configuration1.setServicePeriodMs(23456, true));
    TEST_ASSERT_EQUAL(23456, configuration1.getServicePeriodMs());

    // Simulate controller reset: new facade & sensor, then load saved value
    ConfigurationFacade configuration2;
    auto sensor2 = SensorFacade::create(std::shared_ptr<SmallTransmit>(new MockTransmit()));
    configuration2.plugSensorFacade(sensor2);
    int saved = configuration2.readOption("service_period_ms");
    sensor2->setServicePeriodMs(saved);
    TEST_ASSERT_EQUAL(23456, sensor2->getServicePeriodMs());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_null_sensor_facade);
    RUN_TEST(test_set_service_period_no_save_and_reset);
    RUN_TEST(test_set_service_period_with_save_and_reset);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
