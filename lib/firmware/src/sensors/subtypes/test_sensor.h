#ifndef TESTESENSOR_H
#define TESTESENSOR_H

#include "sensor.h"

class TestSensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
