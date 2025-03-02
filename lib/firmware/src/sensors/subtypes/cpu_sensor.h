#ifndef CPU_H
#define CPU_H

#include "sensor.h"

class CPUSensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
