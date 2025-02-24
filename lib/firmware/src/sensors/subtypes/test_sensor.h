#ifndef TESTESENSOR_H
#define TESTESENSOR_H

#include "sensors/subtypes/sensor.h"

class TestSensor : public Sensor {
    public:
        std::string getSensorData() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
