#include "configuration_facade.h"

OperationResult ConfigurationFacade::setLoraTransmit(std::shared_ptr<LoraTransmit> loraTransmit) {
    this->loraTransmit = loraTransmit;
}

OperationResult ConfigurationFacade::setPassthroughCommunication(std::shared_ptr<PassthroughCommunication> passthroughCommunication) {
    this->passthroughCommunication = passthroughCommunication;
}

OperationResult ConfigurationFacade::setSensorFacade(std::shared_ptr<SensorFacade> sensorFacade) {
    this->sensorFacade = sensorFacade;
}

OperationResult ConfigurationFacade::setServiceCommunication(std::shared_ptr<ServiceCommunication> serviceCommunication) {
    this->serviceCommunication = serviceCommunication;
}