#ifndef BME280_H
#define BME280_H

#include "sensor.h"

class BME280Sensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
