#include "sensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/loraTransmit.h>
#include <exchange/transmits/wifiTransmit.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>

const int DEFAULT_SENSOR_PERIOD_MS = 20000; //low value for testing

SensorFacade::SensorFacade() {
}

std::shared_ptr<SensorFacade> SensorFacade::create(std::shared_ptr<JsonTransmit> transmit) {
    auto facade = std::shared_ptr<SensorFacade>(new SensorFacade());
    facade->sensorCommunication = SensorCommunication::create();
    facade->sensorCommunication->subscribe(std::move(transmit));

    facade->timer.get()->setExecuteFunction([facade]() {
       facade->sendAllSensors();
    });
    
    facade->timer.get()->updateTime(10,DEFAULT_SENSOR_PERIOD_MS);
    return facade;
}

void SensorFacade::sendAllSensors() {
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    Serial.println(sensors.size());
    for(auto const& sensor : sensors) {
        messages.add(sensor->getSensorDataJson());
    }
    serializeJson(doc, serializedJson);
    sensorCommunication->transmit(serializedJson, SERVER_ADDRESS);
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors.push_back(std::move(sensor));
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}