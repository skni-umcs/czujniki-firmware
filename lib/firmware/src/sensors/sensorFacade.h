#include "subtypes/sensor.h"
#include <vector>
#include <memory>
#include <exchange/communications/sensorCommunication.h>
#include "time/timer.h"

class SensorFacade {

    std::vector<std::unique_ptr<Sensor>> sensors;
    std::shared_ptr<SensorCommunication> sensorCommunication;
    std::unique_ptr<Timer> timer = std::unique_ptr<Timer>(new Timer());

    public:
        SensorFacade();
        void sendAllSensors();
        void addSensor(std::unique_ptr<Sensor> sensor);
        int sensorsCount();
};