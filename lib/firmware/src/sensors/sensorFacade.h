#include "subtypes/sensor.h"
#include <vector>
#include <memory>
#include <exchange/communications/sensorCommunication.h>

class SensorFacade {

    std::vector<std::unique_ptr<Sensor>> sensors;
    std::shared_ptr<SensorCommunication> sensorCommunication;

    public:
        SensorFacade();
        void sendAllSensors();
        void addSensor(std::unique_ptr<Sensor> sensor);
        int sensorsCount();
};