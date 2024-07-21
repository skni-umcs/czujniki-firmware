#include "sensor.h"
#include <vector>
#include <memory>

class SensorFacade {

    std::vector<std::unique_ptr<Sensor>> sensors;

    public:
        void sendAllSensors();
        void sendSensorData(std::unique_ptr<Sensor> const& sensor);
        void addSensor(std::unique_ptr<Sensor> sensor);
};