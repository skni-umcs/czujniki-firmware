#include <unity.h>
#include <utils/logger.h>

void test_log() {
    Logger::log("test");
}

void test_logf() {
    Logger::logf("%d times a text %s", 4, "test");
}

void setup() {
    UNITY_BEGIN();
    Logger::setWifi(std::shared_ptr<WifiTransmit>(new WifiTransmit()));
    RUN_TEST(test_log);
    RUN_TEST(test_logf);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
