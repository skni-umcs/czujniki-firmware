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

uint32_t delayMStest = 0;
auto testTransmit = std::shared_ptr<TestTransmit>{new TestTransmit()};

void test_add_sensor() {
    std::shared_ptr<SensorFacade> sensorFacade = SensorFacade::create(testTransmit, false);
    std::unique_ptr<Sensor> testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor(&delayMStest);
    sensorFacade->addSensor(testSensor);
    TEST_ASSERT_EQUAL(1, sensorFacade->sensorsCount());
}

void test_sensor_facade() {
    std::shared_ptr<SensorFacade> sensorFacade = SensorFacade::create(testTransmit, false);
    std::unique_ptr<Sensor> testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor(&delayMStest);
    sensorFacade->addSensor(testSensor);
    sensorFacade->sendAllSensors();
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
