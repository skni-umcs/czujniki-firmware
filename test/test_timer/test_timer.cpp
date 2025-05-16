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

void testDeleteTaskAfterTimeUpdate() {
    std::shared_ptr<Timer> timer = Timer::create();
    for(int i = 0;i<50;++i) {
        timer.get()->updateTime(100000);
    }
    UBaseType_t numberOfTasks = uxTaskGetNumberOfTasks();
    TEST_ASSERT_LESS_OR_EQUAL(10, numberOfTasks); //the default background tasks are usually 6-8
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(testDeleteTaskAfterTimeUpdate);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
