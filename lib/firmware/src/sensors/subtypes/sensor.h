#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <map>

class Sensor 
{
    public:
        virtual std::map<std::string, std::string> getSensorData() = 0;
        virtual void setupSensor(uint32_t* delayMS) = 0;
};

#endif