#ifndef BME280_H
#define BME280_H

#include "sensor.h"

class BME280Sensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
