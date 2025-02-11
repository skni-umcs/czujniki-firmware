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

std::shared_ptr<SensorFacade> SensorFacade::create(std::shared_ptr<JsonTransmit> transmit) {
    auto facade = std::shared_ptr<SensorFacade>(new SensorFacade());
    facade->sensorCommunication = SensorCommunication::create();
    facade->sensorCommunication->subscribe(std::move(transmit));

    facade->setupSensors(transmit);

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
    PacketMessage packetMessage = PacketMessage(TransmissionCode::SENSOR_READING, serializedJson);
    sensorCommunication->transmit(packetMessage.getJson(), SERVER_ADDRESS);
}

OperationResult SensorFacade::setupSensors(std::shared_ptr<JsonTransmit> baseTransmit) {
    #if defined(esp32firebeetle) || defined(mini_test)
        std::unique_ptr<Sensor> mainSensor = std::unique_ptr<TestSensor>(new TestSensor());
        mainSensor->setupSensor(&delayMS);
        addSensor(mainSensor);
    #else
        std::unique_ptr<Sensor> mainSensor = std::unique_ptr<HumidityTemperatureSensor>(new HumidityTemperatureSensor());
        mainSensor->setupSensor(&delayMS);
        addSensor(mainSensor);
        std::unique_ptr<Sensor> cpuSensor = std::unique_ptr<CPUSensor>(new CPUSensor());
        cpuSensor->setupSensor(&delayMS);
        addSensor(cpuSensor);
        std::unique_ptr<NoiseSensor> noiseSensor = std::unique_ptr<NoiseSensor>(new NoiseSensor());
        std::shared_ptr<LoraTransmit> transmit = std::static_pointer_cast<LoraTransmit>(baseTransmit);
        noiseSensor->setLoraTransmit(transmit);
        noiseSensor->setupSensor(&delayMS);
        addSensor(noiseSensor);
    #endif
    return OperationResult::SUCCESS;
}

template <typename T>
void SensorFacade::addSensor(std::unique_ptr<T>& sensor) {
    sensors.push_back(std::move(sensor));
}

int SensorFacade::sensorsCount() {
    return this->sensors.size();
}