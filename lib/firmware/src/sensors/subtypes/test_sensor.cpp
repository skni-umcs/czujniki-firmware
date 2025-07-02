#include "test_sensor.h"
#include <string>
#include <Adafruit_Sensor.h>
#include <string>
#include <sstream>

OperationResult TestSensor::setupSensor() {
    return OperationResult::SUCCESS;
}

std::map<std::string, std::string> TestSensor::getSensorData() {
    std::map<std::string, std::string> resultMap;
    resultMap.insert(std::make_pair("random prime number", std::to_string(5)));
    
    return resultMap;
}