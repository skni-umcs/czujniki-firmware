#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "noise_sensor.h"
#include <sstream>

const std::string NOISE_CODE = "n";

NoiseSensor::NoiseSensor(std::shared_ptr<LoraTransmit> transmit) {
  this->transmit = transmit;
}

OperationResult NoiseSensor::setupSensor() {
  if(transmit == nullptr) {
    Serial.println("Can't read noise, no lora transmit");
    return OperationResult::NOT_FOUND;
  }
  transmit->updateNoise();
  return OperationResult::SUCCESS;
}

std::map<std::string, std::string> NoiseSensor::getSensorData() {
  std::map<std::string, std::string> resultMap;
  if(transmit == nullptr) {
    Serial.println("Can't read noise, no lora transmit");
    return resultMap;
  }
  int noise = transmit->getNoise();
  
  resultMap.insert(std::make_pair(NOISE_CODE, std::to_string(noise)));

  return resultMap;
}
