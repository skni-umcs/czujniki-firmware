#include <unity.h>
#include <WString.h>
#include "utils/string_utils.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

void test_convert_to_wstring() {
    String wstring = "ómessage";
    std::string out = fromWString(wstring);
    std::string expected = "??message";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_convert_to_wstring);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
