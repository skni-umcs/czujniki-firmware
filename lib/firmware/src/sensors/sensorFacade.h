#include "subtypes/sensor.h"
#include <vector>
#include <memory>
#include <exchange/communications/sensorCommunication.h>
#include "time/timer.h"

class SensorFacade : public std::enable_shared_from_this<SensorFacade> {

    std::vector<std::unique_ptr<Sensor>> sensors;
    std::shared_ptr<SensorCommunication> sensorCommunication;
    std::shared_ptr<Timer> timer = Timer::create();
    
    SensorFacade();
    public:
        static std::shared_ptr<SensorFacade> create(std::shared_ptr<JsonTransmit> transmit);
        void sendAllSensors();
        template <typename T>
        void addSensor(std::unique_ptr<T>& sensor);
        int sensorsCount();
        OperationResult setupSensors(std::shared_ptr<JsonTransmit> transmit);
};