#include "SensorFacade.h"
#include <Adafruit_Sensor.h>
#include <memory>

void SensorFacade::sendAllSensors() {
    Serial.println("Read begin");
    for(auto const& sensor : sensors) {
        sendSensorData(sensor);
    }
    Serial.println("Read end");
}

void SensorFacade::sendSensorData(std::unique_ptr<Sensor> const& sensor) {
    Serial.println(sensor->getSensorDataJson().c_str());
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors.push_back(std::move(sensor));
}
