#ifndef SENSORFACADE_H
#define SENSORFACADE_H

#include "subtypes/sensor.h"
#include <vector>
#include <memory>
#include <exchange/communications/sensor_communication.h>
#include "time/timer.h"
#include <exchange/communications/service_communication.h>

const int DEFAULT_SERVICE_PERIOD_MS = 60000*2;
const int DEFAULT_TELEMETRY_PERIOD_MS = DEFAULT_SERVICE_PERIOD_MS*2;

class SensorFacade : public std::enable_shared_from_this<SensorFacade> {

    std::vector<std::unique_ptr<Sensor>> telemetrySensors;
    std::vector<std::unique_ptr<Sensor>> serviceSensors;
    std::shared_ptr<SensorCommunication> telemetryCommunication;
    std::shared_ptr<ServiceCommunication> serviceCommunication;
    std::shared_ptr<Timer> telemetryTimer = Timer::create();
    std::shared_ptr<Timer> serviceTimer  = Timer::create();
    int servicePeriodMs = DEFAULT_SERVICE_PERIOD_MS;
    int telemetryPeriodMs = DEFAULT_TELEMETRY_PERIOD_MS;
    
    SensorFacade();
    public:
        static std::shared_ptr<SensorFacade> create(
            std::shared_ptr<SmallTransmit> transmit, 
            std::shared_ptr<SensorCommunication> telemetryCommunication,
            std::shared_ptr<ServiceCommunication> serviceCommunication,
            bool shouldSetupSensors = true
        );
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
        int telemetryCount();
        int serviceCount();
        OperationResult setupTelemetry();
        OperationResult setupService(std::shared_ptr<SmallTransmit> baseTransmit);
        OperationResult setServicePeriodMs(int servicePeriodMs);
        OperationResult setTelemetryPeriodMs(int telemetryPeriodMs);
        int getServicePeriodMs();
        int getTelemetryPeriodMs();
};

#endif
