#include "configuration_facade.h"
#include "logger.h"
#include "preferences_constant.h"

OperationResult ConfigurationFacade::setLoraTransmit(std::shared_ptr<LoraTransmit> loraTransmit) {
    this->loraTransmit = loraTransmit;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::setPassthroughCommunication(std::shared_ptr<PassthroughCommunication> passthroughCommunication) {
    this->passthroughCommunication = passthroughCommunication;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::setSensorFacade(std::shared_ptr<SensorFacade> sensorFacade) {
    this->sensorFacade = sensorFacade;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::setServiceCommunication(std::shared_ptr<ServiceCommunication> serviceCommunication) {
    this->serviceCommunication = serviceCommunication;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::saveOption(const char* optionKey, int value) {
    preferences.putUShort(optionKey, value);
    return OperationResult::SUCCESS;
}

int ConfigurationFacade::readOption(const char* optionKey) {
    return preferences.getUShort(optionKey);
}

OperationResult ConfigurationFacade::setServicePeriodMs(int servicePeriodMs, bool shouldSave) {
    const char* optionName = "service_period_ms";

    if(this->sensorFacade == nullptr) {
        Logger::log("Configuration facade doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setServicePeriodMs(servicePeriodMs);
    if(shouldSave) {
        saveOption(optionName, servicePeriodMs);
    }
}

OperationResult ConfigurationFacade::setTelemetryPeriodMs(int servicePeriodMs, bool shouldSave) {
    const char* optionName = "telemetry_period_ms";

    if(this->sensorFacade == nullptr) {
        Logger::log("Configuration facade doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setTelemetryPeriodMs(servicePeriodMs);
    if(shouldSave) {
        saveOption(optionName, servicePeriodMs);
    }
}

int ConfigurationFacade::getServicePeriodMs() {
    if(this->sensorFacade == nullptr) {
        return CONFIGURATION_ERROR_INT;
    }
    return this->sensorFacade->getServicePeriodMs();
}

int ConfigurationFacade::getTelemetryPeriodMs() {
    if(this->sensorFacade == nullptr) {
        return CONFIGURATION_ERROR_INT;
    }
    return this->sensorFacade->getTelemetryPeriodMs();
}
