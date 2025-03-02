#include "sensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/loraTransmit.h>
#include <exchange/transmits/wifiTransmit.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>
#include <utils/packetUtils.h>
#include <memory>
#include <vector>
#include <sensors/subtypes/bme_280_sensor.h>
#include <sensors/subtypes/bmp_sensor.h>
#include <sensors/subtypes/cpu_sensor.h>
#include <sensors/subtypes/humidity_temperature_sensor.h>
#include <sensors/subtypes/noise_sensor.h>
#include <sensors/subtypes/test_sensor.h>
#include <sensors/subtypes/sensor.h>

const int DEFAULT_SENSOR_PERIOD_MS = 20000; //low value for testing
uint32_t delayMS = 1000;

SensorFacade::SensorFacade() {
}

std::shared_ptr<SensorFacade> SensorFacade::create(std::shared_ptr<JsonTransmit> transmit, bool shouldSetupSensors) {
    auto facade = std::shared_ptr<SensorFacade>(new SensorFacade());
    facade->sensorCommunication = SensorCommunication::create();
    facade->sensorCommunication->subscribe(std::move(transmit));

    if(shouldSetupSensors) {
        facade->setupSensors(transmit);
    }

    facade->timer.get()->setExecuteFunction([facade]() {
       facade->sendAllSensors();
    });
    
    facade->timer.get()->updateTime(10,DEFAULT_SENSOR_PERIOD_MS);
    return facade;
}

std::string SensorFacade::getAllSensorsMessage() {
    JsonDocument doc;
    JsonObject messages = doc.to<JsonObject>();
    std::string serializedJson;

    for(auto const& sensor : sensors) {
        std::map<std::string, std::string> map = sensor->getSensorData();
        for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
            messages[it->first] = it->second;
        }
    }
    serializeJson(doc, serializedJson);
    return serializedJson;
}

void SensorFacade::sendAllSensors() {
    PacketMessage packetMessage = PacketMessage(TransmissionCode::SENSOR_READING, getAllSensorsMessage());
    sensorCommunication->transmit(packetMessage.getJson(), SERVER_ADDRESS);
}

OperationResult SensorFacade::setupSensors(std::shared_ptr<JsonTransmit> baseTransmit) {
    std::vector<std::unique_ptr<Sensor>> sensors = {};
    #if defined(esp32firebeetle) || defined(mini_test)
        sensors.push_back(std::unique_ptr<TestSensor>(new TestSensor()));
    #else
        sensors.push_back(std::unique_ptr<BMPSensor>(new BMPSensor()));
        sensors.push_back(std::unique_ptr<HumidityTemperatureSensor>(new HumidityTemperatureSensor()));
        sensors.push_back(std::unique_ptr<CPUSensor>(new CPUSensor()));
        std::shared_ptr<LoraTransmit> transmit = std::static_pointer_cast<LoraTransmit>(baseTransmit);
        sensors.push_back(std::unique_ptr<NoiseSensor>(new NoiseSensor(transmit)));
    #endif
    for(std::unique_ptr<Sensor> & sensor : sensors) {
        OperationResult setupResult = sensor->setupSensor();
        if(setupResult == OperationResult::SUCCESS) {
            addSensor(sensor);
        }
    }
    return OperationResult::SUCCESS;
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}

