#ifndef HUMIDITYTEMPERATURESENSOR_H
#define HUMIDITYTEMPERATURESENSOR_H

#include "sensor.h"

class HumidityTemperatureSensor : public Sensor {
    public:
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
