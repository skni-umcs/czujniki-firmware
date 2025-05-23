#ifndef CONFIGURATIONFACADE_H
#define CONFIGURATIONFACADE_H

#include "operation_result.h"
#include <memory>
#include "exchange/transmits/lora_transmit.h"
#include <exchange/communications/passthrough_communication.h>
#include <sensors/sensor_facade.h>

class ConfigurationFacade {
    std::shared_ptr<LoraTransmit> loraTransmit = nullptr;
    std::shared_ptr<PassthroughCommunication> passthroughCommunication = nullptr;
    std::shared_ptr<SensorFacade> sensorFacade = nullptr;
    std::shared_ptr<ServiceCommunication> serviceCommunication = nullptr;

    public:
        OperationResult setLoraTransmit(std::shared_ptr<LoraTransmit> loraTransmit);
        OperationResult setPassthroughCommunication(std::shared_ptr<PassthroughCommunication> passthroughCommunication);
        OperationResult setSensorFacade(std::shared_ptr<SensorFacade> sensorFacade);
        OperationResult setServiceCommunication(std::shared_ptr<ServiceCommunication> serviceCommunication);
};

#endif