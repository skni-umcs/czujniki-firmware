#include "sensor_facade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/lora_transmit.h>
#include <exchange/transmits/wifi_transmit.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>
#include "message/message_decode_utils.h"
#include <memory>
#include <vector>
#include <sensors/subtypes/bme_280_sensor.h>
#include <sensors/subtypes/bme_680_sensor.h>
#include <sensors/subtypes/cpu_sensor.h>
#include <sensors/subtypes/lora_sensor.h>
#include <sensors/subtypes/test_sensor.h>
#include <sensors/subtypes/sensor.h>
#include <sensors/subtypes/heap_sensor.h>
#include <message/message_content.h>
#include "subtypes/bme_constants.h"
#include <utils/logger.h>

#define enableBME() pinMode(BME_POWER_PIN, OUTPUT); digitalWrite(BME_POWER_PIN, HIGH); delay(1000);
#define disableBME() digitalWrite(BME_POWER_PIN, LOW); pinMode(BME_POWER_PIN, INPUT);

uint32_t delayMS = 1000;
#define TELEMETRY_DELAY_MS 20000

SensorFacade::SensorFacade() {
}

std::shared_ptr<SensorFacade> SensorFacade::create(
    std::shared_ptr<SmallTransmit> transmit, 
    std::shared_ptr<SensorCommunication> telemetryCommunication,
    std::shared_ptr<ServiceCommunication> serviceCommunication,
    bool shouldSetupSensors
) {
    auto facade = std::shared_ptr<SensorFacade>(new SensorFacade());
    facade->telemetryCommunication = telemetryCommunication;
    facade->telemetryCommunication->subscribe(transmit);
    facade->serviceCommunication = serviceCommunication;
    facade->serviceCommunication->subscribe(transmit);

    if(shouldSetupSensors) {
        facade->setupTelemetry();
        facade->setupService(transmit);
    }

    facade->serviceTimer.get()->setExecuteFunction([facade]() {
       facade->sendService();
    });
    facade->serviceTimer.get()->updateTime(facade->getServicePeriodMs());

    facade->telemetryTimer.get()->setExecuteFunction([facade]() {
        facade->sendTelemetry();
    });

    delay(TELEMETRY_DELAY_MS);
    facade->telemetryTimer.get()->updateTime(facade->getTelemetryPeriodMs());

    enableBME();

    return facade;
}

std::string SensorFacade::getTelemetry() {
    JsonDocument doc;
    JsonObject messages = doc.to<JsonObject>();
    std::string serializedJson;

    for(auto const& sensor : telemetrySensors) {
        try {
            std::map<std::string, std::string> map = sensor->getSensorData();
            for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
                messages[it->first] = it->second;
            }
        }
        catch (int error) {
            Logger::logf("EXCEPTION in telemetry sensor message with code: %d", error);
        }
    }
    
    serializeJson(doc, serializedJson);
    return serializedJson;
}

std::string SensorFacade::getService() {
    JsonDocument doc;
    JsonObject messages = doc.to<JsonObject>();
    std::string serializedJson;

    for(auto const& sensor : serviceSensors) {
        try {
            std::map<std::string, std::string> map = sensor->getSensorData();
            for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
                messages[it->first] = it->second;
            }
        }
        catch (int error) {
            Logger::logf("EXCEPTION in service sensor message with code: %d", error);
        }
    }
    
    serializeJson(doc, serializedJson);
    return serializedJson;
}

OperationResult SensorFacade::sendTelemetry() {
    MessageContent messageContent = MessageContent(TransmissionCode::TELEMETRY_READING, getTelemetry());
    return telemetryCommunication->transmit(messageContent.getJson(), SERVER_ADDRESS);
}

OperationResult SensorFacade::sendService() {
    MessageContent messageContent = MessageContent(TransmissionCode::SERVICE_READING, getService());
    return telemetryCommunication->transmit(messageContent.getJson(), SERVER_ADDRESS);
}

OperationResult SensorFacade::setupTelemetry() {
    std::vector<std::unique_ptr<Sensor>> sensorCandidates = {};
    #if defined(esp32firebeetle) || defined(mini_test)
        sensorCandidates.push_back(std::unique_ptr<TestSensor>(new TestSensor()));
    #else
        sensorCandidates.push_back(std::unique_ptr<BME280Sensor>(new BME280Sensor()));
        sensorCandidates.push_back(std::unique_ptr<BME680Sensor>(new BME680Sensor()));
    #endif
    for(std::unique_ptr<Sensor> & sensor : sensorCandidates) {
        OperationResult setupResult = sensor->setupSensor();
        if(setupResult == OperationResult::SUCCESS) {
            addTelemetry(sensor);
        }
        
    }
    Logger::logf("Size of telemetry sensors %i\n", telemetrySensors.size());
    return OperationResult::SUCCESS;
}

OperationResult SensorFacade::setupService(std::shared_ptr<SmallTransmit> baseTransmit) {
    std::vector<std::unique_ptr<Sensor>> sensorCandidates = {};
    #if defined(esp32firebeetle) || defined(mini_test)
        
    #else
        sensorCandidates.push_back(std::unique_ptr<CPUSensor>(new CPUSensor()));
        std::shared_ptr<LoraTransmit> transmit = std::static_pointer_cast<LoraTransmit>(baseTransmit);
        sensorCandidates.push_back(std::unique_ptr<LoraSensor>(new LoraSensor(transmit)));
    #endif
    sensorCandidates.push_back(std::unique_ptr<HeapSensor>(new HeapSensor()));
    for(std::unique_ptr<Sensor> & sensor : sensorCandidates) {
        OperationResult setupResult = sensor->setupSensor();
        if(setupResult == OperationResult::SUCCESS) {
            addService(sensor);
        }
        
    }
    Logger::logf("Size of service sensors %i\n", telemetrySensors.size());
    return OperationResult::SUCCESS;
}

int SensorFacade::telemetryCount() {
    return this->telemetrySensors.size();
}

int SensorFacade::serviceCount() {
    return this->serviceSensors.size();
}

OperationResult SensorFacade::setServicePeriodMs(int servicePeriodMs) {
    this->servicePeriodMs = servicePeriodMs;
    serviceTimer.get()->updateTime(servicePeriodMs);
    return OperationResult::SUCCESS;
}

OperationResult SensorFacade::setTelemetryPeriodMs(int telemetryPeriodMs) {
    this->telemetryPeriodMs = telemetryPeriodMs;
    telemetryTimer.get()->updateTime(telemetryPeriodMs);
    return OperationResult::SUCCESS;
}

int SensorFacade::getServicePeriodMs() {
    return servicePeriodMs;
}

int SensorFacade::getTelemetryPeriodMs() {
    return telemetryPeriodMs;
}
