#include "subtypes/sensor.h"
#include <vector>
#include <memory>
#include <exchange/communications/sensor_communication.h>
#include "time/timer.h"
#include <exchange/communications/service_communication.h>

class SensorFacade : public std::enable_shared_from_this<SensorFacade> {

    std::vector<std::unique_ptr<Sensor>> telemetrySensors;
    std::vector<std::unique_ptr<Sensor>> serviceSensors;
    std::shared_ptr<SensorCommunication> telemetryCommunication;
    std::shared_ptr<ServiceCommunication> serviceCommunication;
    std::shared_ptr<Timer> telemetryTimer = Timer::create();
    std::shared_ptr<Timer> serviceTimer  = Timer::create();
    
    SensorFacade();
    public:
        static std::shared_ptr<SensorFacade> create(std::shared_ptr<SmallTransmit> transmit, bool shouldSetupSensors = true);
        std::string getTelemetry();
        std::string getService();
        OperationResult sendTelemetry();
        OperationResult sendService();
        template <typename T>
        OperationResult addTelemetry(std::unique_ptr<T>& sensor) {
            telemetrySensors.push_back(std::move(sensor));
            return OperationResult::SUCCESS;
        }
        template <typename T>
        OperationResult addService(std::unique_ptr<T>& sensor) {
            serviceSensors.push_back(std::move(sensor));
            return OperationResult::SUCCESS;
        }
        int temeletryCount();
        int serviceCount();
        OperationResult setupTelemetry(std::shared_ptr<SmallTransmit> baseTransmit);
        OperationResult setupService(std::shared_ptr<SmallTransmit> baseTransmit);
};