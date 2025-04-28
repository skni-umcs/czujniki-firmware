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
#include <sensors/subtypes/bmp_sensor.h>
#include <sensors/subtypes/cpu_sensor.h>
#include <sensors/subtypes/humidity_temperature_sensor.h>
#include <sensors/subtypes/noise_sensor.h>
#include <sensors/subtypes/test_sensor.h>
#include <sensors/subtypes/sensor.h>
#include <sensors/subtypes/heap_sensor.h>
#include <message/message_content.h>
#include "subtypes/bme_constants.h"

#define enableBME() digitalWrite(BME_POWER_PIN, HIGH);
#define disableBME() digitalWrite(BME_POWER_PIN, LOW);

const int DEFAULT_SENSOR_PERIOD_MS = 32000;
uint32_t delayMS = 1000;

SensorFacade::SensorFacade() {
}

std::shared_ptr<SensorFacade> SensorFacade::create(std::shared_ptr<SmallTransmit> transmit, bool shouldSetupSensors) {
    auto facade = std::shared_ptr<SensorFacade>(new SensorFacade());
    facade->sensorCommunication = SensorCommunication::create();
    facade->sensorCommunication->subscribe(transmit);

    if(shouldSetupSensors) {
        facade->setupSensors(transmit);
    }

    facade->timer.get()->setExecuteFunction([facade]() {
       facade->sendAllSensors();
    });
    
    facade->timer.get()->updateTime(DEFAULT_SENSOR_PERIOD_MS);

    pinMode(BME_POWER_PIN, OUTPUT);
    disableBME();

    return facade;
}

std::string SensorFacade::getAllSensorsMessage() {
    //TODO: prevent faulty sensor from crashing
    JsonDocument doc;
    JsonObject messages = doc.to<JsonObject>();
    std::string serializedJson;

    enableBME();
    for(auto const& sensor : sensors) {
        try {
            std::map<std::string, std::string> map = sensor->getSensorData();
            for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
                messages[it->first] = it->second;
            }
        }
        catch (int error) {
            Serial.printf("EXCEPTION in sensor message with code: %d", error);
        }
    }
    disableBME();
    
    serializeJson(doc, serializedJson);
    return serializedJson;
}

void SensorFacade::sendAllSensors() {
    MessageContent messageContent = MessageContent(TransmissionCode::SENSOR_READING, getAllSensorsMessage());
    sensorCommunication->transmit(messageContent.getJson(), SERVER_ADDRESS);
}

OperationResult SensorFacade::setupSensors(std::shared_ptr<SmallTransmit> baseTransmit) {
    std::vector<std::unique_ptr<Sensor>> sensorCandidates = {};
    #if defined(esp32firebeetle) || defined(mini_test)
        sensorCandidates.push_back(std::unique_ptr<TestSensor>(new TestSensor()));
    #else
        sensorCandidates.push_back(std::unique_ptr<BMPSensor>(new BMPSensor()));
        sensorCandidates.push_back(std::unique_ptr<HumidityTemperatureSensor>(new HumidityTemperatureSensor()));
        sensorCandidates.push_back(std::unique_ptr<BME280Sensor>(new BME280Sensor()));
        sensorCandidates.push_back(std::unique_ptr<BME680Sensor>(new BME680Sensor()));
        sensorCandidates.push_back(std::unique_ptr<CPUSensor>(new CPUSensor()));
        std::shared_ptr<LoraTransmit> transmit = std::static_pointer_cast<LoraTransmit>(baseTransmit);
        sensorCandidates.push_back(std::unique_ptr<NoiseSensor>(new NoiseSensor(transmit)));
    #endif
    sensorCandidates.push_back(std::unique_ptr<HeapSensor>(new HeapSensor()));
    for(std::unique_ptr<Sensor> & sensor : sensorCandidates) {
        OperationResult setupResult = sensor->setupSensor();
        if(setupResult == OperationResult::SUCCESS) {
            addSensor(sensor);
        }
        
    }
    Serial.printf("Size of sensors %i\n", sensors.size());
    return OperationResult::SUCCESS;
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}

