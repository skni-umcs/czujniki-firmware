#ifndef HUMIDITYTEMPERATURESENSOR_H
#define HUMIDITYTEMPERATURESENSOR_H

#include "sensor.h"

class HumidityTemperatureSensor : public Sensor {
    public:
        std::string getSensorDataJson() override;
        void setupSensor(uint32_t* delayMS) override;
};

#endif
