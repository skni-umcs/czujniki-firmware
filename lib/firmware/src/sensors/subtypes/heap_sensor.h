#ifndef HEAPSENSOR_H
#define HEAPSENSOR_H

#include "sensor.h"

class HeapSensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
