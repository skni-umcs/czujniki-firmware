#include <unity.h>
#include <utils/configuration_facade.h>

ConfigurationFacade configurationFacade = ConfigurationFacade();

void test_service_period_ms() {
    int testVal = 5;
    configurationFacade.setServicePeriodMs(testVal);
    TEST_ASSERT_EQUAL(testVal, configurationFacade.getServicePeriodMs());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_service_period_ms);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
