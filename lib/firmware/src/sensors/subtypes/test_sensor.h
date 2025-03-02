#ifndef TESTESENSOR_H
#define TESTESENSOR_H

#include "sensors/subtypes/sensor.h"

class TestSensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
