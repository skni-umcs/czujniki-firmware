 #ifndef SENSOR_H
 #define SENSOR_H

#include <string>

class Sensor 
{
    public:
        virtual std::string getSensorDataJson() = 0;
        virtual void setupSensor(uint32_t* delayMS) = 0;
};

 #endif