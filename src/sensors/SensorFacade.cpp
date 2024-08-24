#include "sensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>
#include <exchange/transmits/loraTransmit.h>
#include <exchange/transmits/wifiTransmit.h>

SensorFacade::SensorFacade() {
    sensorCommunication = SensorCommunication::create();
    std::unique_ptr<JsonTransmit> transmit = std::unique_ptr<LoraTransmit>(new LoraTransmit());
    std::unique_ptr<JsonTransmit> transmitTwo = std::unique_ptr<WifiTransmit>(new WifiTransmit());
    sensorCommunication->subscribe(std::move(transmit));
    sensorCommunication->subscribe(std::move(transmitTwo));
}

void SensorFacade::sendAllSensors() {
    for(auto const& sensor : sensors) {
        sendSensorData(sensor);
    }
}

void SensorFacade::sendSensorData(std::unique_ptr<Sensor> const& sensor) {
    sensorCommunication->transmit(sensor->getSensorDataJson());
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors.push_back(std::move(sensor));
}
