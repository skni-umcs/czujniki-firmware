#ifndef BMP_H
#define BMP_H

#include "sensor.h"

class BMPSensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
