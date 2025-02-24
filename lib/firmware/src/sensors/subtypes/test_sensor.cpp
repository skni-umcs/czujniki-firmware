#include "test_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>
#include <sstream>

void TestSensor::setupSensor(uint32_t* delayMS) {
}

std::map<std::string, std::string> TestSensor::getSensorData() {
    std::map<std::string, std::string> resultMap;
    resultMap.insert(std::make_pair("random prime number", std::to_string(2137)));
    
    return resultMap;
}