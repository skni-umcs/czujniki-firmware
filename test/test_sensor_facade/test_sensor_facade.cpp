#include <unity.h>
#include <string>
#include <algorithm> 
#include <exchange/communications/jsonCommunication.h>
#include <exchange/transmits/wifiTransmit.h>
#include <exchange/transmits/loraTransmit.h>
#include <iostream>
#include <memory>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include "sensors/sensorFacade.h"
#include "test_sensor.h"
#include "test_transmit.h"

uint32_t delayMS = 0;
auto testTransmit = std::shared_ptr<TestTransmit>{new TestTransmit()};

void test_add_sensor() {
    SensorFacade sensorFacade = SensorFacade(testTransmit);
    auto testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor(&delayMS);
    sensorFacade.addSensor(std::move(testSensor));
    TEST_ASSERT_EQUAL(1, sensorFacade.sensorsCount());
}

void test_sensor_facade() {
    SensorFacade sensorFacade = SensorFacade(testTransmit);
    auto testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor(&delayMS);
    sensorFacade.addSensor(std::move(testSensor));
    sensorFacade.sendAllSensors();
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_add_sensor);
    RUN_TEST(test_sensor_facade);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
