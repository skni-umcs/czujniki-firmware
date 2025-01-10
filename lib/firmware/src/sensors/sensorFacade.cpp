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

    facade->h = 966;

    std::weak_ptr<SensorFacade> weakFacade(facade);
    Serial.println("weak facade");
    Serial.println((int)weakFacade.lock().get());
    facade->timer.get()->setExecuteFunction([facade]() {
        Serial.println("executing");
        /*auto obj = weakFacade.lock();
        Serial.println((int)obj.get());
        if(obj) {
            obj->sendAllSensors();
        }*/
       facade->sendAllSensors();
    });
    Serial.printf("thisp ointer w ocnstrukerze: %p", facade);
    facade->timer.get()->updateTime(10,DEFAULT_SENSOR_PERIOD_MS);

    Serial.printf("Wskaznik kontsr komunikacji: %i", (int)facade->sensorCommunication.get());
    facade->h = 2137;
    return facade;
}

void SensorFacade::sendAllSensors() {
    Serial.printf("liczba h: %i\n", h);
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    Serial.println(sensors.size());
    for(auto const& sensor : sensors) {
        messages.add(sensor->getSensorDataJson());
    }
    serializeJson(doc, serializedJson);
    Serial.printf("Wskaznik thisa: %p", (int)this);
    Serial.printf("Wskaznik komunikacji: %p", (int)this->sensorCommunication.get());
    sensorCommunication->transmit(serializedJson, SERVER_ADDRESS);
}

void SensorFacade::addSensor(std::unique_ptr<Sensor> sensor) {
    Serial.printf("Wskaznik addsensor1 komunikacji: %p", (int)sensorCommunication.get());
    sensors.push_back(std::move(sensor));
    Serial.printf("Wskaznik addsensor2 komunikacji: %p a sensorow jest %i", (int)sensorCommunication.get(), sensors.size());
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}