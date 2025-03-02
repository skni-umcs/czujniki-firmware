#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <map>
#include <utils/operationResult.h>

class Sensor 
{
    public:
        virtual std::map<std::string, std::string> getSensorData() = 0;
        virtual OperationResult setupSensor() = 0;
};

#endif