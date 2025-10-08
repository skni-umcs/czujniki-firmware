#ifndef SENSORFACADESENSOR_H
#define SENSORFACADESENSOR_H

#include "sensor.h"
#include "memory"
#include "../sensor_facade.h"

class SensorFacadeSensor : public Sensor {

    std::shared_ptr<SensorFacade> facade;

    public:
        SensorFacadeSensor(std::shared_ptr<SensorFacade> facade);
        std::map<std::string, std::string> getSensorData() override;
        OperationResult setupSensor() override;
};

#endif
