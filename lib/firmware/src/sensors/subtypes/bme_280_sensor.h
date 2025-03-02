#ifndef BME280_H
#define BME280_H

#include "sensor.h"

class BME280Sensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
