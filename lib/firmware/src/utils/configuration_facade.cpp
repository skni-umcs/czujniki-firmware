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
    std::cout << this->sensorFacade->getServicePeriodMs() << std::endl;
    setServicePeriodMs(readOption(SERVICE_PERIOD_OPTION), NO_SAVE);
    std::cout << this->sensorFacade->getServicePeriodMs() << std::endl;
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
    if(!preferences.isKey(optionKey)) {
        return CONFIGURATION_ERROR_INT;
    }
    return preferences.getUShort(optionKey);
}

OperationResult ConfigurationFacade::setServicePeriodMs(int servicePeriodMs, bool shouldSave) {
    std::cout << this->sensorFacade << std::endl;
    if(servicePeriodMs == CONFIGURATION_ERROR_INT) {
        Logger::log("Set service period ms got error as period to set");
        return OperationResult::ERROR;
    }
    if(this->sensorFacade == nullptr) {
        Logger::log("Set service period doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setServicePeriodMs(servicePeriodMs);\
    if(shouldSave) {
        saveOption(SERVICE_PERIOD_OPTION, servicePeriodMs);
    }
    return OperationResult::SUCCESS;
}

OperationResult ConfigurationFacade::setTelemetryPeriodMs(int telemetryPeriodMs, bool shouldSave) {
    if(telemetryPeriodMs == CONFIGURATION_ERROR_INT) {
        Logger::log("Set telemetry period ms got error as period to set");
        return OperationResult::ERROR;
    }
    if(this->sensorFacade == nullptr) {
        Logger::log("Set telemetry period doesn't have a sensor facade pointer");
        return OperationResult::ERROR;
    }
    this->sensorFacade->setTelemetryPeriodMs(telemetryPeriodMs);
    if(shouldSave) {
        saveOption(TELEMETRY_PERIOD_OPTION, telemetryPeriodMs);
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
