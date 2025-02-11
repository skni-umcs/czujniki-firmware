#ifndef CPU_H
#define CPU_H

#include "sensor.h"

class CPUSensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
