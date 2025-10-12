#ifndef CONFIGURATIONFACADE_H
#define CONFIGURATIONFACADE_H

#include "operation_result.h"
#include <memory>
#include "exchange/transmits/lora_transmit.h"
#include <exchange/communications/passthrough_communication.h>
#include <sensors/sensor_facade.h>

#define CONFIGURATION_ERROR_INT -1

class ConfigurationFacade {
    std::shared_ptr<LoraTransmit> loraTransmit = nullptr;
    std::shared_ptr<PassthroughCommunication> passthroughCommunication = nullptr;
    std::shared_ptr<SensorFacade> sensorFacade = nullptr;
    std::shared_ptr<ServiceCommunication> serviceCommunication = nullptr;

    public:
        OperationResult init();
        OperationResult plugLoraTransmit(std::shared_ptr<LoraTransmit> loraTransmit);
        OperationResult plugPassthroughCommunication(std::shared_ptr<PassthroughCommunication> passthroughCommunication);
        OperationResult plugSensorFacade(std::shared_ptr<SensorFacade> sensorFacade);
        OperationResult plugServiceCommunication(std::shared_ptr<ServiceCommunication> serviceCommunication);
        OperationResult saveOption(const char* optionKey, int value);
        int readOption(const char* optionKey);

        //sensor facade
        OperationResult setServicePeriodMs(int servicePeriodMs, bool shouldSave = true);
        OperationResult setTelemetryPeriodMs(int telemetryPeriodMs, bool shouldSave = true);
        int getServicePeriodMs();
        int getTelemetryPeriodMs();

        //service communication
        OperationResult setAskTimeoutMs(int askTimeoutMs, bool shouldSave = true);
        OperationResult setTimeSyncPeriodMs(int timeSyncPeriodMs, bool shouldSave = true);
        int getAskTimeoutMs();
        int getTimeSyncPeriodMs();
};

#endif