#include "sensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/loraTransmit.h>
#include <exchange/transmits/wifiTransmit.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>

SensorFacade::SensorFacade() {
    sensorCommunication = SensorCommunication::create();
    std::unique_ptr<JsonTransmit> transmit = std::unique_ptr<LoraTransmit>(new LoraTransmit());
    std::unique_ptr<JsonTransmit> transmitTwo = std::unique_ptr<WifiTransmit>(new WifiTransmit());
    sensorCommunication->subscribe(std::move(transmit));
    sensorCommunication->subscribe(std::move(transmitTwo));
}

void SensorFacade::sendAllSensors() {
    DynamicJsonDocument doc(1024);
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    for(auto const& sensor : sensors) {
        messages.add(sensor->getSensorDataJson());
    }
    serializeJson(doc, serializedJson);
    sensorCommunication->transmit(serializedJson);
}

void SensorFacade::sendSensorData(std::unique_ptr<Sensor> const& sensor) {
    
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors.push_back(std::move(sensor));
}
