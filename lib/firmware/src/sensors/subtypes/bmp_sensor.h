#ifndef BMP_H
#define BMP_H

#include "sensor.h"

class BMPSensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
