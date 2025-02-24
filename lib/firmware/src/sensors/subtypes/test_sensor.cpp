#include "test_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>
#include <sstream>

void TestSensor::setupSensor(uint32_t* delayMS) {
}

std::string TestSensor::getSensorData() {
    // Get temperature event and print its value.
    sensors_event_t event;
    std::stringstream result;
    result << "random prime number: 2137";
    return result.str();
}