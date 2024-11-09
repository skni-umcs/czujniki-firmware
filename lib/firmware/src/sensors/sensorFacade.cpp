#include "sensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/loraTransmit.h>
#include <exchange/transmits/wifiTransmit.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>

const int DEFAULT_SENSOR_PERIOD_MS = 60000;

SensorFacade::SensorFacade() {
    sensorCommunication = SensorCommunication::create();
    std::unique_ptr<JsonTransmit> transmit = std::unique_ptr<LoraTransmit>(new LoraTransmit());
    sensorCommunication->subscribe(std::move(transmit));

    timer.get()->setExecuteFunction([this]() {this->sendAllSensors();});
    timer.get()->updateTime(10,DEFAULT_SENSOR_PERIOD_MS);
}

void SensorFacade::sendAllSensors() {
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    for(auto const& sensor : sensors) {
        messages.add(sensor->getSensorDataJson());
    }
    serializeJson(doc, serializedJson);
    sensorCommunication->transmit(serializedJson);
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors.push_back(std::move(sensor));
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}