#include "configuration_facade.h"
#include "logger.h"
#include "preferences_constant.h"

const char* SERVICE_PERIOD_OPTION = "service_period_ms";
const char* TELEMETRY_PERIOD_OPTION = "telemetry_period_ms";
#define NO_SAVE false

OperationResult ConfigurationFacade::plugLoraTransmit(std::shared_ptr<LoraTransmit> loraTransmit) {
    this->loraTransmit = loraTransmit;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::plugPassthroughCommunication(std::shared_ptr<PassthroughCommunication> passthroughCommunication) {
    this->passthroughCommunication = passthroughCommunication;
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::plugSensorFacade(std::shared_ptr<SensorFacade> sensorFacade) {
    this->sensorFacade = sensorFacade;
    setServicePeriodMs(readOption(SERVICE_PERIOD_OPTION), NO_SAVE);
    setTelemetryPeriodMs(readOption(TELEMETRY_PERIOD_OPTION), NO_SAVE);
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::plugServiceCommunication(std::shared_ptr<ServiceCommunication> serviceCommunication) {
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
    std::cout << this->sensorFacade << std::endl;
    if(this->sensorFacade == nullptr) {
        Logger::log("Configuration facade doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setServicePeriodMs(servicePeriodMs);\
    if(shouldSave) {
        saveOption(SERVICE_PERIOD_OPTION, servicePeriodMs);
    }
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::setTelemetryPeriodMs(int servicePeriodMs, bool shouldSave) {
    if(this->sensorFacade == nullptr) {
        Logger::log("Configuration facade doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setTelemetryPeriodMs(servicePeriodMs);
    if(shouldSave) {
        saveOption(TELEMETRY_PERIOD_OPTION, servicePeriodMs);
    }
    return OperationResult::SUCCESS;
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
