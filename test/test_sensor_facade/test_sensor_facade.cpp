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
#include "sensors/sensor_facade.h"
#include "sensors/subtypes/test_sensor.h"
#include "test_transmit.h"

auto testTransmit = std::shared_ptr<TestTransmit>{new TestTransmit()};

void test_add_sensor() {
    std::shared_ptr<SensorFacade> sensorFacade = SensorFacade::create(testTransmit, SensorCommunication::create(), ServiceCommunication::create(), false);
    std::unique_ptr<Sensor> testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor();
    sensorFacade->addService(testSensor);
    TEST_ASSERT_EQUAL(1, sensorFacade->serviceCount());

    std::unique_ptr<Sensor> testTelemetry = std::unique_ptr<TestSensor>(new TestSensor());
    testTelemetry->setupSensor();
    sensorFacade->addTelemetry(testTelemetry);
    TEST_ASSERT_EQUAL(1, sensorFacade->telemetryCount());
}

void test_sensor_message() {
    std::shared_ptr<SensorFacade> sensorFacade = SensorFacade::create(testTransmit, SensorCommunication::create(), ServiceCommunication::create(), false);
    std::unique_ptr<Sensor> testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor();
    sensorFacade->addTelemetry(testSensor);
    std::string result = sensorFacade->getTelemetry();

    std::string expected = "{\"random prime number\":\"2137\"}";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), result.c_str());
}

void test_sensor_facade() {
    std::shared_ptr<SensorFacade> sensorFacade = SensorFacade::create(testTransmit, SensorCommunication::create(), ServiceCommunication::create(), false);
    std::unique_ptr<Sensor> testSensor = std::unique_ptr<TestSensor>(new TestSensor());
    testSensor->setupSensor();
    sensorFacade->addTelemetry(testSensor);
    sensorFacade->sendTelemetry();
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_add_sensor);
    RUN_TEST(test_sensor_message);
    RUN_TEST(test_sensor_facade);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
